#include "jaot/ir_lowering.h"

#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include <vector>

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
            class Node {
                int value;
                boolean marked;
                Link next;
            }

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

            class Link {
                Node previous;
            }
        )";

        JAOT::Lexer lexer(source);
        JAOT::Parser parser(lexer.lex());
        const JAOT::Program ast = parser.parse();

        require(ast.classes.size() == 3,
    "expected three class declarations");

        require(ast.classes[0].name == "Node",
            "first class should be Node");
        require(ast.classes[0].fields.size() == 3,
            "Node should have three fields");
        require(ast.classes[0].fields[0].typeName == "int" &&
                ast.classes[0].fields[0].name == "value",
            "first Node field should be int value");
        require(ast.classes[0].fields[1].typeName == "boolean" &&
                ast.classes[0].fields[1].name == "marked",
            "second Node field should be boolean marked");
        require(ast.classes[0].fields[2].typeName == "Link" &&
                ast.classes[0].fields[2].name == "next",
            "third Node field should be Link next");

        require(ast.classes[1].name == "Main",
            "second class should be Main");
        require(ast.classes[1].fields.empty(),
            "Main should have no fields");
        require(ast.classes[1].methods.size() == 2,
            "Main should have two methods");

        require(ast.classes[2].name == "Link",
            "third class should be Link");
        require(ast.classes[2].fields.size() == 1 &&
                ast.classes[2].fields[0].typeName == "Node",
            "Link should have a Node reference field");

        JAOT::IrLowerer lowerer;
        const JAOT::IR::Program &ir = lowerer.lower(ast);

        require(ir.classes.size() == 3,
    "expected three IR classes");

        require(ir.classes[0].name == "Node",
            "first IR class should be Node");
        require(ir.classes[0].fields.size() == 3,
            "IR Node should have three fields");
        require(ir.classes[0].fields[0].name == "value" &&
                ir.classes[0].fields[0].typeName == "int",
            "first IR Node field should be int value");
        require(ir.classes[0].fields[1].name == "marked" &&
                ir.classes[0].fields[1].typeName == "boolean",
            "second IR Node field should be boolean marked");
        require(ir.classes[0].fields[2].name == "next" &&
                ir.classes[0].fields[2].typeName == "Link",
            "third IR Node field should be Link next");

        require(ir.classes[1].name == "Main" &&
                ir.classes[1].fields.empty(),
            "IR Main should have no fields");

        require(ir.classes[2].name == "Link" &&
                ir.classes[2].fields.size() == 1 &&
                ir.classes[2].fields[0].name == "previous" &&
                ir.classes[2].fields[0].typeName == "Node",
            "IR Link should have a Node reference field");

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
        require(add.blocks.size() == 1,
            "'add' should lower to one basic block");

        const JAOT::IR::BasicBlock &addEntry = add.blocks[0];
        require(addEntry.id == 0,
            "'add' entry block should have ID 0");
        require(addEntry.statements.empty(),
            "'add' return should be represented by its terminator");
        require(addEntry.terminator.kind == JAOT::IR::TerminatorKind::Return,
            "'add' entry block should end with a return");
        require(addEntry.terminator.expression != nullptr,
            "'add' return should have an expression");

        const JAOT::IR::Expr &sum = *addEntry.terminator.expression;
        require(sum.kind == JAOT::IR::ExprKind::Binary,
            "'add' return statement should be a binary expression");
        require(sum.binaryOperator == "+",
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
        require(main.blocks.size() == 1,
            "'main' should lower to one basic block");

        const JAOT::IR::BasicBlock &mainEntry = main.blocks[0];
        require(mainEntry.id == 0,
            "'main' entry block should have ID 0");
        require(mainEntry.statements.size() == 2,
            "'main' should lower to assignment and call statements");
        require(mainEntry.terminator.kind == JAOT::IR::TerminatorKind::Return,
            "'main' should end with an implicit return");
        require(mainEntry.terminator.expression == nullptr,
            "implicit void return should not have an expression");

        const JAOT::IR::Statement &assignment = mainEntry.statements[0];
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
        require(call.arguments[0]->integer == 20,
            "first 'add' argument should have value 20");
        require(call.arguments[1]->kind == JAOT::IR::ExprKind::Integer,
            "second 'add' argument should be 22");
        require(call.arguments[1]->integer == 22,
            "second 'add' argument should have value 22");

        const JAOT::IR::Statement &printStatement = mainEntry.statements[1];
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



        const std::string objectExpressionSource = R"(
            class Main {
                static void main() {
                    new Node();
                    null;
                }
            }

            class Node {
            }
        )";

        JAOT::Lexer objectExpressionLexer(objectExpressionSource);
        JAOT::Parser objectExpressionParser(objectExpressionLexer.lex());
        const JAOT::Program objectExpressionAst = objectExpressionParser.parse();

        require(objectExpressionAst.classes.size() == 2,
            "object-expression source should parse two classes");
        require(objectExpressionAst.classes[0].methods.size() == 1,
            "Main should contain main");
        require(objectExpressionAst.classes[0].methods[0].body.size() == 2,
            "main should contain both object expressions");

        const JAOT::Stmt &newStatement =
            objectExpressionAst.classes[0].methods[0].body[0];
        require(newStatement.kind == JAOT::StmtKind::Expression &&
                newStatement.expression &&
                newStatement.expression->kind == JAOT::ExprKind::NewObject &&
                newStatement.expression->className == "Node",
            "new Node() should parse as a Node allocation expression");

        const JAOT::Stmt &nullStatement =
            objectExpressionAst.classes[0].methods[0].body[1];
        require(nullStatement.kind == JAOT::StmtKind::Expression &&
                nullStatement.expression &&
                nullStatement.expression->kind == JAOT::ExprKind::Null,
            "null should parse as a null expression");



        return 0;
    } catch (const std::exception &error) {
        std::cerr << error.what() << '\n';
        return 1;
    }
}
