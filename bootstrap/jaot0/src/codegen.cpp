#include "jaot/codegen.h"
#include "jaot/expression_codegen.h"
#include "jaot/frame.h"
#include "jaot/machine.h"
#include "jaot/runtime_codegen.h"

#include <array>
#include <cstddef>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace JAOT {
    namespace {
        constexpr std::array<const char *, 6> ArgumentRegisters = {
            "%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"
        };
        constexpr std::array<const char *, 6> ArgumentRegisters32 = {
            "%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"
        };

        class FunctionGenerator {
        public:
            FunctionGenerator(
                    std::ostream &out,
                    const std::unordered_map<std::string, MethodSignature> &methods,
                    const std::unordered_map<std::string, IR::Class> &classes
            ) : out_(out),
                machine_(out),
                runtime_(machine_),
                methods_(methods),
                classes_(classes) {
            }

            void generate(const IR::Method &method) {
                methodName_ = methodSymbol(method.signature);
                currentClass_ = method.signature.ownerClass;
                frame_.build(method);

                const int frameSize = frame_.frameSize();

                machine_.beginFunction("jaot_" + methodName_, frameSize);
                ExpressionLowerer expressions(
                    machine_,
                    runtime_,
                    frame_,
                    methods_,
                    classes_,
                    methodName_
                );

                if (method.parameters.size() > ArgumentRegisters32.size()) {
                    throw std::runtime_error(
                        "JAOT0 functions support at most six parameters");
                }

                for (std::size_t i = 0; i < method.parameters.size(); ++i) {
                    const IR::LocalId parameter = method.parameters[i];
                    if (frame_.type(parameter).kind == Type::Reference) {
                        out_ << "    movq " << ArgumentRegisters[i] << ", "
                             << frame_.offset(parameter) << "(%rbp)\n";
                    } else {
                        out_ << "    movl " << ArgumentRegisters32[i] << ", "
                             << frame_.offset(parameter) << "(%rbp)\n";
                    }
                }

                for (const IR::BasicBlock &block : method.blocks) {
                    machine_.label(
                        ".Ljaot_" + methodName_ +
                        "_block_" + std::to_string(block.id)
                    );

                    for (const IR::Statement &statement : block.statements) {
                        generateStatement(statement, expressions);
                    }

                    generateTerminator(block.terminator, expressions);
                }
            }

        private:

            void generateStatement(
                const IR::Statement &statement,
                ExpressionLowerer &expressions
            ) {
                switch (statement.kind) {
                    case IR::StatementKind::AssignLocal: {
                        if (!statement.expression) {
                            throw std::runtime_error(
                                "IR local assignment without value");
                        }
                        const ValueLocation value =
                            expressions.lower(*statement.expression);
                        machine_.storeFrameSlot(
                            frame_.offset(statement.target),
                            value
                        );
                        return;
                    }

                    case IR::StatementKind::AssignField: {
                        if (!statement.targetExpression ||
                            statement.targetExpression->kind != IR::ExprKind::FieldAccess ||
                            !statement.targetExpression->left ||
                            !statement.expression) {
                            throw std::runtime_error("incomplete IR field assignment");
                        }

                        const IR::Expr &target = *statement.targetExpression;
                        const ValueLocation receiver =
                            expressions.lower(*target.left);
                        machine_.push(receiver);

                        const ValueLocation value =
                            expressions.lower(*statement.expression);

                        machine_.emitRaw(
                            value.kind == ValueKind::Reference64
                                ? "    movq %rax, %rdx\n"
                                : "    movl %eax, %edx\n"
                        );
                        const ValueLocation restoredReceiver =
                            machine_.pop(ValueKind::Reference64);

                        const std::size_t nullLabel = fieldLabelCounter_++;
                        const std::string nonNullLabel =
                            ".Ljaot_" + methodName_ +
                            "_field_nonnull_" + std::to_string(nullLabel);

                        machine_.jumpIfNotZero(
                            restoredReceiver,
                            nonNullLabel
                        );
                        runtime_.failNullDereference();

                        machine_.label(nonNullLabel);
                        machine_.emitRaw(
                            value.kind == ValueKind::Reference64
                                ? "    movq %rdx, "
                                : "    movl %edx, "
                        );
                        machine_.emitRaw(
                            std::to_string(target.fieldOffset) +
                            "(%rax)\n"
                        );

                        return;
                    }

                    case IR::StatementKind::Evaluate:
                        if (!statement.expression) {
                            throw std::runtime_error(
                                "IR expression statement without expression");
                        }
                        expressions.lower(*statement.expression);
                        return;
                }

                throw std::runtime_error("unknown IR statement kind");
            }

            void generateTerminator(
                const IR::Terminator &terminator,
                ExpressionLowerer &expressions
            ) {
                switch (terminator.kind) {
                    case IR::TerminatorKind::Jump:
                        machine_.jump(
                            ".Ljaot_" + methodName_ +
                            "_block_" + std::to_string(terminator.target)
                        );
                        return;

                    case IR::TerminatorKind::Branch: {
                        if (!terminator.condition) {
                            throw std::runtime_error("IR branch without condition");
                        }

                        const ValueLocation condition =
                            expressions.lower(*terminator.condition);
                        machine_.jumpIfNotZero(
                            condition,
                            ".Ljaot_" + methodName_ +
                            "_block_" + std::to_string(terminator.trueTarget)
                        );
                        machine_.jump(
                            ".Ljaot_" + methodName_ +
                            "_block_" + std::to_string(terminator.falseTarget)
                        );
                        return;
                    }

                    case IR::TerminatorKind::Return:
                        if (terminator.expression) {
                            expressions.lower(*terminator.expression);
                        } else {
                            machine_.emitIntConstant(0);
                        }

                        machine_.endFunction();
                        return;
                }
                throw std::runtime_error("unknown IR terminator kind");
            }

            std::ostream &out_;
            Machine machine_;
            RuntimeEmitter runtime_;
            const std::unordered_map<std::string, MethodSignature> &methods_;
            const std::unordered_map<std::string, IR::Class> &classes_;
            FrameLayout frame_;
            std::string methodName_;
            std::string currentClass_;
            std::size_t fieldLabelCounter_ = 0;
        };
    }

    void CodeGenerator::generate(const IR::Program &program, std::ostream &out) {
        out << "# generated by jaot0\n\n";

        std::unordered_map<std::string, MethodSignature> methods;
        std::unordered_map<std::string, IR::Class> classes;
        for (const IR::Class &classDeclaration : program.classes) {
            if (!classes.emplace(
                    classDeclaration.name,
                    classDeclaration).second) {
                throw std::runtime_error(
                    "duplicate class: " + classDeclaration.name);
            }
        }

        for (const IR::Method &method : program.methods) {
            if (!methods.emplace(
                method.signature.ownerClass + "." + method.signature.name,
                method.signature).second) {
                throw std::runtime_error(
                "duplicate function: " +
                method.signature.ownerClass + "." +
                method.signature.name);
            }
        }

        for (const IR::Method &method : program.methods) {
            FunctionGenerator generator(out, methods, classes);
            generator.generate(method);
            out << '\n';
        }
    }
} // namespace JAOT
