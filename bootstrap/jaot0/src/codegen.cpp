#include "jaot/codegen.h"

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
            ) : out_(out), methods_(methods), classes_(classes) {
            }

            void generate(const IR::Method &method) {
                methodName_ = methodSymbol(method.signature);
                currentClass_ = method.signature.ownerClass;
                collectLocals(method);

                const int frameSize = (stackSize_ + 15) & ~15;

                out_ << ".text\n";
                out_ << ".globl jaot_" << methodName_ << "\n";
                out_ << ".type jaot_" << methodName_ << ", @function\n";
                out_ << "jaot_" << methodName_ << ":\n";
                out_ << "    pushq %rbp\n";
                out_ << "    movq %rsp, %rbp\n";

                if (frameSize > 0) {
                    out_ << "    subq $" << frameSize << ", %rsp\n";
                }

                if (method.parameters.size() > ArgumentRegisters32.size()) {
                    throw std::runtime_error(
                        "JAOT0 functions support at most six parameters");
                }

                for (std::size_t i = 0; i < method.parameters.size(); ++i) {
                    const IR::LocalId parameter = method.parameters[i];
                    if (localType(parameter).kind == Type::Reference) {
                        out_ << "    movq " << ArgumentRegisters[i] << ", "
                             << localOffset(parameter) << "(%rbp)\n";
                    } else {
                        out_ << "    movl " << ArgumentRegisters32[i] << ", "
                             << localOffset(parameter) << "(%rbp)\n";
                    }
                }

                for (const IR::BasicBlock &block : method.blocks) {
                    out_ << ".Ljaot_" << methodName_
                         << "_block_" << block.id << ":\n";

                    for (const IR::Statement &statement : block.statements) {
                        generateStatement(statement);
                    }

                    generateTerminator(block.terminator);
                }
            }

        private:
            static int localSize(const TypeRef &type) {
                return type.kind == Type::Reference ? 8 : 4;
            }

            void collectLocals(const IR::Method &method) {
                for (const IR::Local &local : method.locals) {
                    const int size = localSize(local.type);
                    const int offset = -stackSize_ - size;
                    if (!locals_.emplace(local.id, offset).second) {
                        throw std::runtime_error("duplicate IR local id");
                    }
                    localTypes_.emplace(local.id, local.type);
                    stackSize_ += size;
                }

                for (const IR::LocalId parameter : method.parameters) {
                    if (!locals_.contains(parameter)) {
                        throw std::runtime_error(
                            "method parameter has no IR local slot");
                    }
                }
            }

            void generateStatement(const IR::Statement &statement) {
                switch (statement.kind) {
                    case IR::StatementKind::AssignLocal:
                        if (!statement.expression) {
                            throw std::runtime_error(
                                "IR local assignment without value");
                        }
                        generateExpression(*statement.expression);
                        out_ << (localType(statement.target).kind == Type::Reference
                                     ? "    movq %rax, "
                                     : "    movl %eax, ")
                             << localOffset(statement.target)
                             << "(%rbp)\n";
                        return;

                    case IR::StatementKind::AssignField: {
                        if (!statement.targetExpression ||
                            statement.targetExpression->kind != IR::ExprKind::FieldAccess ||
                            !statement.targetExpression->left ||
                            !statement.expression) {
                            throw std::runtime_error("incomplete IR field assignment");
                        }

                        const IR::Expr &target = *statement.targetExpression;
                        generateExpression(*target.left);

                        out_ << "    pushq %rax\n";
                        stackDepth_ += 8;

                        generateExpression(*statement.expression);

                        out_ << (target.type.kind == Type::Reference
                            ? "    movq %rax, %rdx\n"
                            : "    movl %eax, %edx\n");
                        out_ << "    popq %rcx\n";
                        stackDepth_ -= 8;

                        const std::size_t nullLabel = fieldLabelCounter_++;
                        out_ << "    testq %rcx, %rcx\n";
                        out_ << "    jne .Ljaot_" << methodName_ << "_nonnull_" << nullLabel << "\n";

                        emitCall("jaot_null_dereference");

                        out_ << ".Ljaot_" << methodName_ << "_nonnull_" << nullLabel << ":\n";
                        out_ << (target.type.kind == Type::Reference
                            ? "    movq %rdx, "
                            : "    movl %edx, ")
                            << target.fieldOffset << "(%rcx)\n";

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
                        out_ << "    jmp .Ljaot_" << methodName_
                             << "_block_" << terminator.target << "\n";
                        return;

                    case IR::TerminatorKind::Branch:
                        if (!terminator.condition) {
                            throw std::runtime_error("IR branch without condition");
                        }

                        generateExpression(*terminator.condition);
                        out_ << "    testl %eax, %eax\n";
                        out_ << "    jne .Ljaot_" << methodName_ << "_block_" << terminator.trueTarget << "\n";
                        out_ << "    jmp .Ljaot_" << methodName_ << "_block_" << terminator.falseTarget << "\n";
                        return;

                    case IR::TerminatorKind::Return:
                        if (terminator.expression) {
                            generateExpression(*terminator.expression);
                        } else {
                            out_ << "    movl $0, %eax\n";
                        }

                        out_ << "    leave\n";
                        out_ << "    ret\n";
                        return;
                }
                throw std::runtime_error("unknown IR terminator kind");
            }

            void generateExpression(const IR::Expr &expression) {
                switch (expression.kind) {
                    case IR::ExprKind::Integer:
                        out_ << "    movl $" << expression.integer
                             << ", %eax\n";
                        return;

                    case IR::ExprKind::Boolean:
                        out_ << "    movl $" << (expression.boolean ? 1 : 0) << ", %eax\n";
                        return;

                    case IR::ExprKind::Null:
                        out_ << "    xorq %rax, %rax\n";
                        return;

                    case IR::ExprKind::NewObject: {
                        const auto classFound = classes_.find(expression.className);
                        if (classFound == classes_.end()) {
                            throw std::runtime_error(
                                "unknown class: " + expression.className
                            );
                        }
                        out_ << "    movq $" << classFound->second.size
                             << ", %rdi\n";
                        emitCall("jaot_allocate");
                        return;
                    }

                    case IR::ExprKind::Local:
                        if (expression.type.kind == Type::Reference) {
                            out_ << "    movq " << localOffset(expression.local)
                                 << "(%rbp), %rax\n";
                        } else {
                            out_ << "    movl " << localOffset(expression.local)
                                 << "(%rbp), %eax\n";
                        }
                        return;

                    case IR::ExprKind::FieldAccess: {
                        if (!expression.left) {
                            throw std::runtime_error(
                                "incomplete IR field access"
                            );
                        }

                        generateExpression(*expression.left);

                        const std::size_t nullLabel = fieldLabelCounter_++;

                        out_ << "    testq %rax, %rax\n";
                        out_ << "    jne .Ljaot_" << methodName_ << "_nonnull_" << nullLabel << "\n";

                        emitCall("jaot_null_dereference");

                        out_ << ".Ljaot_" << methodName_ << "_nonnull_" << nullLabel << ":\n";
                        out_ << (expression.type.kind == Type::Reference
                            ? "    movq "
                            : "    movl ")
                            << expression.fieldOffset << "(%rax), "
                            << (expression.type.kind == Type::Reference
                                ? "%rax\n"
                                : "%eax\n");

                        return;
                    }

                    case IR::ExprKind::Unary:
                        if (!expression.operand) {
                            throw std::runtime_error("incomplete IR unary expression");
                        }
                        generateExpression(*expression.operand);
                        if (expression.unaryOperator == "!") {
                            out_ << "    xorl $1, %eax\n";
                            out_ << "    andl $1, %eax\n";
                            return;
                        }
                        throw std::runtime_error("unknown IR unary operator");

                    case IR::ExprKind::Binary:
                        if (!expression.left || !expression.right) {
                            throw std::runtime_error(
                                "incomplete IR binary expression");
                        }
                        generateBinary(expression);
                        return;

                    case IR::ExprKind::Call:
                        generateCall(expression);
                        return;
                }

                throw std::runtime_error("unknown IR expression kind");
            }

            void generateBinary(const IR::Expr &expression) {
                if (expression.binaryOperator == "&&") {
                    const std::string label =
                        ".Ljaot_" + methodName_ + "_logical_and_" +
                        std::to_string(divisionLabelCounter_++);

                    generateExpression(*expression.left);
                    out_ << "    testl %eax, %eax\n";
                    out_ << "    je " << label << "\n";

                    generateExpression(*expression.right);
                    out_ << "    testl %eax, %eax\n";
                    out_ << "    setne %al\n";
                    out_ << "    movzbl %al, %eax\n";
                    out_ << "    jmp " << label << "_done\n";

                    out_ << label << ":\n";
                    out_ << "    movl $0, %eax\n";
                    out_ << label << "_done:\n";
                    return;
                }

                if (expression.binaryOperator == "||") {
                    const std::string label =
                        ".Ljaot_" + methodName_ + "_logical_or_" +
                        std::to_string(divisionLabelCounter_++);

                    generateExpression(*expression.left);
                    out_ << "    testl %eax, %eax\n";
                    out_ << "    jne " << label << "\n";

                    generateExpression(*expression.right);
                    out_ << "    testl %eax, %eax\n";
                    out_ << "    setne %al\n";
                    out_ << "    movzbl %al, %eax\n";
                    out_ << "    jmp " << label << "_done\n";

                    out_ << label << ":\n";
                    out_ << "    movl $1, %eax\n";
                    out_ << label << "_done:\n";
                    return;
                }

                generateExpression(*expression.left);
                out_ << "    pushq %rax\n";
                stackDepth_ += 8;

                generateExpression(*expression.right);
                if (expression.left->type.kind == Type::Reference) {
                    out_ << "    movq %rax, %rcx\n";
                } else {
                    out_ << "    movl %eax, %ecx\n";
                }
                out_ << "    popq %rax\n";
                stackDepth_ -= 8;

                if (expression.binaryOperator == "+") {
                    out_ << "    addl %ecx, %eax\n";
                } else if (expression.binaryOperator == "-") {
                    out_ << "    subl %ecx, %eax\n";
                } else if (expression.binaryOperator == "*") {
                    out_ << "    imull %ecx, %eax\n";
                } else if (expression.binaryOperator == "/") {
                    const std::string label =
                                ".Ljaot_" + methodName_ + "_division_" +
                                std::to_string(divisionLabelCounter_++);
                    const std::string normalLabel = label + "_normal";
                    const std::string overflowLabel =
                            label + "_overflow";
                    const std::string zeroLabel = label + "_zero";
                    const std::string doneLabel = label + "_done";

                    out_ << "    testl %ecx, %ecx\n";
                    out_ << "    je " << zeroLabel << "\n";
                    out_ << "    cmpl $-1, %ecx\n";
                    out_ << "    jne " << normalLabel << "\n";
                    out_ << "    cmpl $-2147483648, %eax\n";
                    out_ << "    je " << overflowLabel << "\n";
                    out_ << normalLabel << ":\n";
                    out_ << "    cltd\n";
                    out_ << "    idivl %ecx\n";
                    out_ << "    jmp " << doneLabel << "\n";
                    out_ << overflowLabel << ":\n";
                    out_ << "    movl $-2147483648, %eax\n";
                    out_ << "    jmp " << doneLabel << "\n";
                    out_ << zeroLabel << ":\n";
                    out_ << "    ud2\n";
                    out_ << doneLabel << ":\n";
                } else {
                    const std::string condition = [&]() -> std::string {
                        if (expression.binaryOperator == "==") return "e";
                        if (expression.binaryOperator == "!=") return "ne";
                        if (expression.binaryOperator == "<") return "l";
                        if (expression.binaryOperator == "<=") return "le";
                        if (expression.binaryOperator == ">") return "g";
                        if (expression.binaryOperator == ">=") return "ge";
                        throw std::runtime_error("unknown IR binary operator");
                    }();

                    if (expression.left->type.kind == Type::Reference) {
                        out_ << "    cmpq %rcx, %rax\n";
                    } else {
                        out_ << "    cmpl %ecx, %eax\n";
                    }
                    out_ << "    set" << condition << " %al\n";
                    out_ << "    movzbl %al, %eax\n";
                }
            }

            void generateCall(const IR::Expr &expression) {
                if (expression.callee.ownerClass == "<runtime>") {
                    if (expression.arguments.size() != 1) {
                        throw std::runtime_error(
                            expression.callee.name + " expects one argument");
                    }

                    generateExpression(*expression.arguments[0]);
                    out_ << "    movl %eax, %edi\n";
                    emitCall(
                        expression.callee.name == "printInt"
                            ? "jaot_print_int"
                            : "jaot_print_bool");
                    return;
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
                    generateExpression(*argument);
                    out_ << "    pushq %rax\n";
                    stackDepth_ += 8;
                }

                for (std::size_t i = expression.arguments.size(); i > 0; --i) {
                    out_ << "    popq " << ArgumentRegisters[i - 1] << "\n";
                    stackDepth_ -= 8;
                }

                emitCall("jaot_" + methodSymbol(method->second));
            }

            void emitCall(const std::string &name) const {
                const bool needsPadding = stackDepth_ % 16 != 0;
                if (needsPadding) {
                    out_ << "    subq $8, %rsp\n";
                }

                out_ << "    call " << name << "\n";

                if (needsPadding) {
                    out_ << "    addq $8, %rsp\n";
                }
            }

            int localOffset(IR::LocalId id) const {
                const auto found = locals_.find(id);
                if (found == locals_.end()) {
                    throw std::runtime_error("unknown IR local id");
                }

                return found->second;
            }

            TypeRef localType(IR::LocalId id) const {
                const auto found = localTypes_.find(id);
                if (found == localTypes_.end()) {
                    throw std::runtime_error("unknown IR local id");
                }
                return found->second;
            }

            std::ostream &out_;
            const std::unordered_map<std::string, MethodSignature> &methods_;
            const std::unordered_map<std::string, IR::Class> &classes_;
            std::unordered_map<IR::LocalId, int> locals_;
            std::unordered_map<IR::LocalId, TypeRef> localTypes_;
            int stackSize_ = 0;
            int stackDepth_ = 0;
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
