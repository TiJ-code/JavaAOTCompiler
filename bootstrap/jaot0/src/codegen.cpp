#include "jaot/codegen.h"
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
                        generateStatement(statement);
                    }

                    generateTerminator(block.terminator);
                }
            }

        private:

            void generateStatement(const IR::Statement &statement) {
                switch (statement.kind) {
                    case IR::StatementKind::AssignLocal: {
                        if (!statement.expression) {
                            throw std::runtime_error(
                                "IR local assignment without value");
                        }
                        const ValueLocation value =
                            generateExpression(*statement.expression);
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
                            generateExpression(*target.left);
                        machine_.push(receiver);

                        const ValueLocation value =
                            generateExpression(*statement.expression);

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
                            "_nonnull_" + std::to_string(nullLabel);

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
                        generateExpression(*statement.expression);
                        return;
                }

                throw std::runtime_error("unknown IR statement kind");
            }

            void generateTerminator(const IR::Terminator &terminator) {
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
                            generateExpression(*terminator.condition);
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
                            generateExpression(*terminator.expression);
                        } else {
                            machine_.emitIntConstant(0);
                        }

                        machine_.endFunction();
                        return;
                }
                throw std::runtime_error("unknown IR terminator kind");
            }

            ValueLocation generateExpression(const IR::Expr &expression) {
                switch (expression.kind) {
                    case IR::ExprKind::Integer:
                        return machine_.emitIntConstant(expression.integer);

                    case IR::ExprKind::Boolean:
                        return machine_.emitBooleanConstant(expression.boolean);

                    case IR::ExprKind::Null:
                        return machine_.emitNull();

                    case IR::ExprKind::NewObject: {
                        const auto classFound = classes_.find(expression.className);
                        if (classFound == classes_.end()) {
                            throw std::runtime_error(
                                "unknown class: " + expression.className
                            );
                        }
                        return runtime_.allocate(classFound->second.size);
                    }

                    case IR::ExprKind::Local:
                        return machine_.loadFrameSlot(
                            frame_.offset(expression.local),
                            expression.type.kind == Type::Reference
                                ? ValueKind::Reference64
                                : ValueKind::Int32
                        );

                    case IR::ExprKind::FieldAccess: {
                        if (!expression.left) {
                            throw std::runtime_error(
                                "incomplete IR field access"
                            );
                        }

                        const ValueLocation receiver =
                            generateExpression(*expression.left);

                        const std::size_t nullLabel = fieldLabelCounter_++;

                        const std::string nonNullLabel =
                            ".Ljaot_" + methodName_ +
                            "_nonnull_" + std::to_string(nullLabel);

                        machine_.jumpIfNotZero(receiver, nonNullLabel);
                        runtime_.failNullDereference();

                        machine_.label(nonNullLabel);
                        machine_.emitRaw(
                            expression.type.kind == Type::Reference
                                ? "    movq "
                                : "    movl "
                        );
                        machine_.emitRaw(
                            std::to_string(expression.fieldOffset) +
                            "(%rax), " +
                            (expression.type.kind == Type::Reference
                                ? "%rax\n"
                                : "%eax\n")
                        );

                        return expression.type.kind == Type::Reference
                            ? ValueLocation::referenceInRax()
                            : ValueLocation::intInEax();
                    }

                    case IR::ExprKind::Unary:
                        if (!expression.operand) {
                            throw std::runtime_error("incomplete IR unary expression");
                        }
                        generateExpression(*expression.operand);
                        if (expression.unaryOperator == "!") {
                            machine_.emitUnaryNot();
                            return ValueLocation::intInEax();
                        }
                        throw std::runtime_error("unknown IR unary operator");

                    case IR::ExprKind::Binary:
                        if (!expression.left || !expression.right) {
                            throw std::runtime_error(
                                "incomplete IR binary expression");
                        }
                        return generateBinary(expression);

                    case IR::ExprKind::Call:
                        return generateCall(expression);
                }

                throw std::runtime_error("unknown IR expression kind");
            }

            ValueLocation generateBinary(const IR::Expr &expression) {
                if (expression.binaryOperator == "&&") {
                    const std::string label =
                        ".Ljaot_" + methodName_ + "_logical_and_" +
                        std::to_string(divisionLabelCounter_++);

                    const ValueLocation left =
                        generateExpression(*expression.left);
                    machine_.jumpIfZero(left, label);

                    generateExpression(*expression.right);
                    machine_.emitRaw("    testl %eax, %eax\n");
                    machine_.emitRaw("    setne %al\n");
                    machine_.emitRaw("    movzbl %al, %eax\n");
                    machine_.jump(label + "_done");

                    machine_.label(label);
                    machine_.emitIntConstant(0);
                    machine_.label(label + "_done");
                    return ValueLocation::intInEax();
                }

                if (expression.binaryOperator == "||") {
                    const std::string label =
                        ".Ljaot_" + methodName_ + "_logical_or_" +
                        std::to_string(divisionLabelCounter_++);

                    const ValueLocation left =
                        generateExpression(*expression.left);
                    machine_.jumpIfNotZero(left, label);

                    generateExpression(*expression.right);
                    machine_.emitRaw("    testl %eax, %eax\n");
                    machine_.emitRaw("    setne %al\n");
                    machine_.emitRaw("    movzbl %al, %eax\n");
                    machine_.jump(label + "_done");

                    machine_.label(label);
                    machine_.emitIntConstant(1);
                    machine_.label(label + "_done");
                    return ValueLocation::intInEax();
                }

                const ValueLocation left =
                    generateExpression(*expression.left);
                machine_.push(left);

                const ValueLocation right =
                    generateExpression(*expression.right);
                if (expression.left->type.kind == Type::Reference) {
                    machine_.emitRaw("    movq %rax, %rcx\n");
                } else {
                    machine_.emitRaw("    movl %eax, %ecx\n");
                }
                const ValueLocation restoredLeft =
                    machine_.pop(left.kind);

                if (expression.binaryOperator == "+") {
                    machine_.emitBinaryIntOperation("+");
                } else if (expression.binaryOperator == "-") {
                    machine_.emitBinaryIntOperation("-");
                } else if (expression.binaryOperator == "*") {
                    machine_.emitBinaryIntOperation("*");
                } else if (expression.binaryOperator == "/") {
                    machine_.emitDivision(
                        methodName_,
                        divisionLabelCounter_++
                    );
                } else {
                    machine_.emitComparison(
                        restoredLeft,
                        right,
                        expression.binaryOperator
                    );
                }

                return ValueLocation::intInEax();
            }

            ValueLocation generateCall(const IR::Expr &expression) {
                if (expression.callee.ownerClass == "<runtime>") {
                    if (expression.arguments.size() != 1) {
                        throw std::runtime_error(
                            expression.callee.name + " expects one argument");
                    }

                    const ValueLocation argument =
                        generateExpression(*expression.arguments[0]);
                    runtime_.print(
                        argument,
                        expression.callee.name == "printBool"
                    );
                    return ValueLocation::intInEax();
                }

                const auto method = methods_.find(
                    expression.callee.ownerClass + "." +
                    expression.callee.name);
                if (method == methods_.end()) {
                    throw std::runtime_error(
                        "unknown function: " + expression.callee.ownerClass +
                        "." + expression.callee.name);
                }

                if (expression.arguments.size() !=
                    method->second.parameterTypes.size()) {
                    throw std::runtime_error(
                        "function " + expression.callee.name + " expects " +
                        std::to_string(method->second.parameterTypes.size()) +
                        " arguments");
                }

                if (expression.receiver) {
                    throw std::runtime_error(
                        "instance method calls are not supported by JAOT0");
                }

                if (expression.arguments.size() > ArgumentRegisters.size()) {
                    throw std::runtime_error(
                        "JAOT0 functions support at most six arguments");
                }

                for (const auto &argument : expression.arguments) {
                    const ValueLocation value =
                        generateExpression(*argument);
                    machine_.push(value);
                }

                machine_.collectArguments(expression.arguments.size());

                machine_.call("jaot_" + methodSymbol(method->second));
                return ValueLocation{
                    expression.type.kind == Type::Reference
                        ? ValueKind::Reference64
                        : ValueKind::Int32,
                    expression.type.kind == Type::Reference
                        ? ValueStorage::Rax
                        : ValueStorage::Eax
                };
            }

            std::ostream &out_;
            Machine machine_;
            RuntimeEmitter runtime_;
            const std::unordered_map<std::string, MethodSignature> &methods_;
            const std::unordered_map<std::string, IR::Class> &classes_;
            FrameLayout frame_;
            std::string methodName_;
            std::string currentClass_;
            std::size_t divisionLabelCounter_ = 0;
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
