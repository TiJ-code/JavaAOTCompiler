#include "jaot/codegen.h"

#include <array>
#include <cstddef>
#include <stdexcept>
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
                    const std::unordered_map<std::string, std::size_t> &methods
            ) : out_(out), methods_(methods) {
            }

            void generate(const Method &method) {
                methodName_ = method.name;
                collectLocals(method);

                const int frameSize = (stackSize_ + 15) & ~15;

                out_ << ".text\n";
                out_ << ".globl jaot_" << method.name << "\n";
                out_ << ".type jaot_" << method.name << ", @function \n";
                out_ << "jaot_" << method.name << ":\n";

                out_ << "    pushq %rbp\n";
                out_ << "    movq %rsp, %rbp\n";

                if (frameSize > 0) {
                    out_ << "    subq $" << frameSize << ", %rsp\n";
                }

                for (std::size_t i = 0; i < method.parameters.size(); ++i) {
                    out_ << "    movl " << ArgumentRegisters32[i] << ", "
                         << localOffset(method.parameters[i].name) << "(%rbp)\n";
                }

                for (const Stmt &statement : method.body) {
                    generateStatement(statement);
                }

                out_ << "    movl $0, %eax\n";
                out_ << "    leave\n";
                out_ << "    ret\n";
            }

        private:
            static constexpr int LocalSize = 4;

            void allocateLocal(const std::string &name) {
                if (locals_.contains(name)) {
                    throw std::runtime_error("duplicate variable: " + name);
                }

                stackSize_ += LocalSize;
                locals_.emplace(name, -stackSize_);
            }

            void collectLocals(const Method &method) {
                if (method.parameters.size() > ArgumentRegisters.size()) {
                    throw std::runtime_error(
                        "JAOT0 functions support at most six parameters");
                }

                for (const Parameter &parameter : method.parameters) {
                    allocateLocal(parameter.name);
                }

                for (const Stmt &statement : method.body) {
                    if (statement.kind != StmtKind::VarDecl) {
                        continue;
                    }

                    allocateLocal(statement.name);
                }
            }

            void generateStatement(const Stmt &statement) {
                switch (statement.kind) {
                    case StmtKind::VarDecl:
                        generateVariableDeclaration(statement);
                        return;

                    case StmtKind::Expression:
                        generateExpression(*statement.expression);
                        return;

                    case StmtKind::Return:
                        if (statement.expression) {
                            generateExpression(*statement.expression);
                        } else {
                            out_ << "    movl $0, %eax\n";
                        }
                        out_ << "    leave\n";
                        out_ << "    ret\n";
                        return;
                }

                throw std::runtime_error("unknown statement kind");
            }

            void generateVariableDeclaration(const Stmt &statement) {
                if (!statement.expression) {
                    throw std::runtime_error("variable declaration without initializer: " + statement.name);
                }

                generateExpression(*statement.expression);

                const int offset = localOffset(statement.name);

                out_ << "    movl %eax, " << offset << "(%rbp)\n";
            }

            void generateExpression(const Expr &expression) {
                switch (expression.kind) {
                    case ExprKind::Integer:
                        out_ << "    movl $" << expression.integer << ", %eax\n";
                        return;

                    case ExprKind::Binary:
                        generateBinary(expression);
                        return;

                    case ExprKind::Call:
                        generateCall(expression);
                        return;

                    case ExprKind::Variable:
                        generateVariable(expression);
                        return;
                }

                throw std::runtime_error("unknown expression kind");
            }

            void generateVariable(const Expr &expression) {
                const int offset = localOffset(expression.name);

                out_ << "    movl " << offset << "(%rbp), %eax\n";
            }

            void generateBinary(const Expr &expression) {
                generateExpression(*expression.left);
                out_ << "    pushq %rax\n";
                stackDepth_ += 8;

                generateExpression(*expression.right);
                out_ << "    movl %eax, %ecx\n";

                out_ << "    popq %rax\n";
                stackDepth_ -= 8;

                switch (expression.op) {
                    case '+':
                        out_ << "    addl %ecx, %eax\n";
                        break;

                    case '-':
                        out_ << "    subl %ecx, %eax\n";
                        break;

                    case '*':
                        out_ << "    imull %ecx, %eax\n";
                        break;

                    case '/':
                    {
                        const std::string label =
                                ".Ljaot_" + methodName_ + "_division_" +
                                std::to_string(divisionLabelCounter_++);
                        const std::string normalLabel = label + "_normal";
                        const std::string overflowLabel = label + "_overflow";
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
                        break;
                    }

                    default:
                        throw std::runtime_error("unknown binary operator");
                }
            }

            void generateCall(const Expr &expression) {
                if (expression.callee == "printInt") {
                    if (expression.arguments.size() != 1) {
                        throw std::runtime_error("printInt expects one argument");
                    }

                    generateExpression(*expression.arguments[0]);

                    out_ << "    movl %eax, %edi\n";
                    emitCall("jaot_print_int");
                    return;
                }

                const auto method = methods_.find(expression.callee);
                if (method == methods_.end()) {
                    throw std::runtime_error("unknown function: " + expression.callee);
                }

                if (expression.arguments.size() != method->second) {
                    throw std::runtime_error(
                        "function " + expression.callee + " expects " +
                        std::to_string(method->second) + " arguments");
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

                emitCall("jaot_" + expression.callee);
            }

            void emitCall(const std::string &name) {
                const bool needsPadding = stackDepth_ % 16 != 0;
                if (needsPadding) {
                    out_ << "    subq $8, %rsp\n";
                }

                out_ << "    call " << name << "\n";

                if (needsPadding) {
                    out_ << "    addq $8, %rsp\n";
                }
            }

            int localOffset(const std::string &name) const {
                const auto it = locals_.find(name);

                if (it == locals_.end()) {
                    throw std::runtime_error("unknown variable: " + name);
                }

                return it->second;
            }

            std::ostream &out_;

            const std::unordered_map<std::string, std::size_t> &methods_;

            std::unordered_map<std::string, int> locals_;

            int stackSize_ = 0;
            int stackDepth_ = 0;
            std::string methodName_;
            std::size_t divisionLabelCounter_ = 0;
        };

    }

    void CodeGenerator::generate(const Program &program, std::ostream &out) {
        out << "# generated by jaot0\n\n";

        std::unordered_map<std::string, std::size_t> methods;
        for (const Method &method : program.methods) {
            if (method.parameters.size() > ArgumentRegisters.size()) {
                throw std::runtime_error(
                    "JAOT0 functions support at most six parameters");
            }

            if (!methods.emplace(method.name, method.parameters.size()).second) {
                throw std::runtime_error("duplicate function: " + method.name);
            }
        }

        for (const Method &method : program.methods) {
            FunctionGenerator generator(out, methods);
            generator.generate(method);
            out << '\n';
        }
    }
}
