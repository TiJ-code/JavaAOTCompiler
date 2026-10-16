#include "jaot/ir_lowering.h"

#include <iostream>
#include <ostream>
#include <stdexcept>

#include "jaot/lexer.h"
#include "jaot/parser.h"

namespace {
    void require(bool condition, const std::string &message) {
        if (!condition) {
            throw std::runtime_error(message);
        }
    }
}

int main() {
    try {
        const std::string source = R"(
            class Main {
                static int add(int a, int b) {
                    return a + b;
                }

                static void main() {
                    int result;
                    result = add(20, 22);
                    printInt(result);
                }
            }
        )";

        JAOT::Lexer lexer(source);
        JAOT::Parser parser(lexer.lex());
        const JAOT::Program ast = parser.parse();

        JAOT::IrLowerer lowerer;
        const JAOT::IR::Program ir = lowerer.lower(ast);

        require(ir.methods.size() == 2,
            "expected two lowered methods");

        const JAOT::IR::Method &add = ir.methods[0];
        require(add.name == "add",
            "expected 'add' method first");
        require(add.returnType == JAOT::Type::Int,
            "'add' should return 'int'");
        require(add.parameters == std::vector<JAOT::IR::LocalId>{0, 1},
            "'add' parameters should map to local IDs 0 and 1");
        require(add.locals.size() == 2,
            "'add' should have two locals");
        require(add.locals[0].name == "a",
            "first 'add' local should be parameter 'a'");
        require(add.locals[1].name == "b",
            "second 'add' local should be parameter 'b'");
        require(add.statements.size() == 1,
            "'add' should lower to one statement");

        const JAOT::IR::Statement &returnStatement = add.statements[0];
        require(returnStatement.kind == JAOT::IR::StatementKind::Return,
            "'add' statement should be a return statement");
        require(returnStatement.expression != nullptr,
            "'add' return should have an expression");

        const JAOT::IR::Expr &sum = *returnStatement.expression;
        require(sum.kind == JAOT::IR::ExprKind::Binary,
            "'add' return statement should be a binary expression");
        require(sum.binaryOperator == '+',
            "'add' should use +");
        require(sum.left &&
            sum.left->kind == JAOT::IR::ExprKind::Local &&
            sum.left->local == 0,
            "left operand should refer to parameter 'a'");
        require(sum.right &&
            sum.right->kind == JAOT::IR::ExprKind::Local &&
            sum.right->local == 1,
            "left operand should refer to parameter 'b'");

        const JAOT::IR::Method &main = ir.methods[1];
        require(main.name == "main",
            "expected 'main' method second");
        require(main.locals.size() == 1 &&
            main.locals[0].name == "result",
            "uninitialized declaration should create a local");
        require(main.statements.size() == 2,
            "'main' should lower to assignment and call statements");

        const JAOT::IR::Statement &assignment = main.statements[0];
        require(assignment.kind == JAOT::IR::StatementKind::AssignLocal,
            "first 'main' statement should be an assignment");
        require(assignment.target == main.locals[0].id,
            "assignment should target 'result'");
        require(assignment.expression &&
            assignment.expression->kind == JAOT::IR::ExprKind::Call,
            "assignment value should be a call");

        const JAOT::IR::Expr &call = *assignment.expression;
        require(call.callee == "add" &&
            call.arguments.size() == 2,
            "assignment should call 'add' with two arguments");
        require(call.arguments[0]->kind == JAOT::IR::ExprKind::Integer,
            "first 'add' argument should be 20");
        require(call.arguments[1]->kind == JAOT::IR::ExprKind::Integer,
            "second 'add' argument should be 22");

        const JAOT::IR::Statement &printStatement = main.statements[1];
        require(printStatement.kind == JAOT::IR::StatementKind::Evaluate,
            "second 'main' statement should be an expression");
        require(printStatement.expression &&
            printStatement.expression->kind == JAOT::IR::ExprKind::Call &&
            printStatement.expression->callee == "printInt",
            "second 'main' statement should call 'printInt'");
        require(printStatement.expression->arguments.size() == 1 &&
            printStatement.expression->arguments[0]->kind == JAOT::IR::ExprKind::Local &&
            printStatement.expression->arguments[0]->local == main.locals[0].id,
            "'printInt' should receive result");

        return 0;
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
