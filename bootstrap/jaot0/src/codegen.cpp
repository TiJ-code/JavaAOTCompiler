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
                    const std::unordered_map<std::string, std::size_t> &methods
            ) : out_(out), methods_(methods) {
            }

            void generate(const IR::Method &method) {
                methodName_ = method.name;
                collectLocals(method);

                const int frameSize = (stackSize_ + 15) & ~15;

                out_ << ".text\n";
                out_ << ".globl jaot_" << method.name << "\n";
                out_ << ".type jaot_" << method.name << ", @function\n";
                out_ << "jaot_" << method.name << ":\n";
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
                    out_ << "    movl " << ArgumentRegisters32[i] << ", "
                         << localOffset(method.parameters[i]) << "(%rbp)\n";
                }

                for (const IR::Statement &statement : method.statements) {
                    generateStatement(statement);
                }

                out_ << "    movl $0, %eax\n";
                out_ << "    leave\n";
                out_ << "    ret\n";
            }

        private:
            static constexpr int LocalSize = 4;

            void collectLocals(const IR::Method &method) {
                for (const IR::Local &local : method.locals) {
                    const int offset = -stackSize_ - LocalSize;
                    if (!locals_.emplace(local.id, offset).second) {
                        throw std::runtime_error("duplicate IR local id");
                    }
                    stackSize_ += LocalSize;
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
                        out_ << "    movl %eax, "
                             << localOffset(statement.target)
                             << "(%rbp)\n";
                        return;

                    case IR::StatementKind::Evaluate:
                        if (!statement.expression) {
                            throw std::runtime_error(
                                "IR expression statement without expression");
                        }
                        generateExpression(*statement.expression);
                        return;

                    case IR::StatementKind::Return:
                        if (statement.expression) {
                            generateExpression(*statement.expression);
                        } else {
                            out_ << "    movl $0, %eax\n";
                        }
                        out_ << "    leave\n";
                        out_ << "    ret\n";
                        return;
                }

                throw std::runtime_error("unknown IR statement kind");
            }

            void generateExpression(const IR::Expr &expression) {
                switch (expression.kind) {
                    case IR::ExprKind::Integer:
                        out_ << "    movl $" << expression.integer
                             << ", %eax\n";
                        return;

                    case IR::ExprKind::Local:
                        out_ << "    movl " << localOffset(expression.local)
                             << "(%rbp), %eax\n";
                        return;

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
                generateExpression(*expression.left);
                out_ << "    pushq %rax\n";
                stackDepth_ += 8;

                generateExpression(*expression.right);
                out_ << "    movl %eax, %ecx\n";
                out_ << "    popq %rax\n";
                stackDepth_ -= 8;

                switch (expression.binaryOperator) {
                    case '+':
                        out_ << "    addl %ecx, %eax\n";
                        break;

                    case '-':
                        out_ << "    subl %ecx, %eax\n";
                        break;

                    case '*':
                        out_ << "    imull %ecx, %eax\n";
                        break;

                    case '/': {
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
                        break;
                    }

                    default:
                        throw std::runtime_error(
                            "unknown IR binary operator");
                }
            }

            void generateCall(const IR::Expr &expression) {
                if (expression.callee == "printInt") {
                    if (expression.arguments.size() != 1) {
                        throw std::runtime_error(
                            "printInt expects one argument");
                    }

                    generateExpression(*expression.arguments[0]);
                    out_ << "    movl %eax, %edi\n";
                    emitCall("jaot_print_int");
                    return;
                }

                const auto method = methods_.find(expression.callee);
                if (method == methods_.end()) {
                    throw std::runtime_error(
                        "unknown function: " + expression.callee);
                }

                if (expression.arguments.size() != method->second) {
                    throw std::runtime_error(
                        "function " + expression.callee + " expects " +
                        std::to_string(method->second) + " arguments");
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

            int localOffset(IR::LocalId id) const {
                const auto found = locals_.find(id);
                if (found == locals_.end()) {
                    throw std::runtime_error("unknown IR local id");
                }

                return found->second;
            }

            std::ostream &out_;
            const std::unordered_map<std::string, std::size_t> &methods_;
            std::unordered_map<IR::LocalId, int> locals_;
            int stackSize_ = 0;
            int stackDepth_ = 0;
            std::string methodName_;
            std::size_t divisionLabelCounter_ = 0;
        };
    }

    void CodeGenerator::generate(const IR::Program &program, std::ostream &out) {
        out << "# generated by jaot0\n\n";

        std::unordered_map<std::string, std::size_t> methods;
        for (const IR::Method &method : program.methods) {
            if (!methods.emplace(method.name, method.parameters.size()).second) {
                throw std::runtime_error("duplicate function: " + method.name);
            }
        }

        for (const IR::Method &method : program.methods) {
            FunctionGenerator generator(out, methods);
            generator.generate(method);
            out << '\n';
        }
    }
} // namespace JAOT
