#include "jaot/codegen.h"

#include <complex>
#include <stdexcept>
#include <unordered_map>

namespace JAOT {
    namespace {

        class FunctionGenerator {
        public:
            explicit FunctionGenerator(std::ostream &out) : out_(out) {
            }

            void generate(const Method &method) {
                collectLocals(method);

                out_ << ".text\n";
                out_ << ".globl jaot_" << method.name << "\n";
                out_ << ".type jaot_" << method.name << ", @function \n";
                out_ << "jaot_" << method.name << ":\n";

                out_ << "    pushq %rbp\n";
                out_ << "    movq %rsp, %rbp\n";

                if (stackSize_ > 0) {
                    out_ << "    subq $" << stackSize_ << ", %rsp\n";
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

            void collectLocals(const Method &method) {
                for (const Stmt &statement : method.body) {
                    if (statement.kind != StmtKind::VarDecl) {
                        continue;
                    }

                    if (locals_.contains(statement.name)) {
                        throw std::runtime_error("duplicate variable: " +
                             statement.name);
                    }

                    stackSize_ += LocalSize;

                    locals_.emplace(statement.name, -stackSize_);
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
                        generateExpression(*statement.expression);
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

                generateExpression(*expression.right);
                out_ << "    movl %eax, %ecx\n";

                out_ << "    popq %rax\n";

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
                    out_ << "    call jaot_print_int\n";
                    return;
                }

                throw std::runtime_error("unknown function: " + expression.callee);
            }

            int localOffset(const std::string &name) const {
                const auto it = locals_.find(name);

                if (it == locals_.end()) {
                    throw std::runtime_error("unknown variable: " + name);
                }

                return it->second;
            }

            std::ostream &out_;

            std::unordered_map<std::string, int> locals_;

            int stackSize_ = 0;
        };

    }

    void CodeGenerator::generate(const Program &program, std::ostream &out) {
        out << "# generated by jaot0\n\n";

        for (const Method &method : program.methods) {
            FunctionGenerator generator(out);
            generator.generate(method);
            out << '\n';
        }
    }
}
