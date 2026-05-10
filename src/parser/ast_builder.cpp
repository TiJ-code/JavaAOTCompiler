#include "parser/parser.h"

#include "JavaLexer.h"
#include "JavaParser.h"
#include "antlr4-runtime.h"

#include "parser/ast_builder.h"
#include "semantic/symbol_table.h"

#include <any>
#include <fstream>

using namespace antlr4;

static ASTNode *to_node( const std::any &a )
{
    try
    {
        return std::any_cast<ASTNode *>(a);
    }
    catch (...)
    {
        return nullptr;
    }
}

ASTNode *parse_java_file(const char *path) {
	std::ifstream file(path);

	if (!file.is_open())
		return nullptr;

	ANTLRInputStream input(file);

	JavaLexer lexer(&input);
	CommonTokenStream tokens(&lexer);

	JavaParser parser(&tokens);

	auto tree = parser.compilationUnit();

	if (parser.getNumberOfSyntaxErrors() > 0)
		return nullptr;

	ASTBuilder builder;
	builder.visit(tree);

	return builder.root;
}

std::any ASTBuilder::visitCompilationUnit(JavaParser::CompilationUnitContext *ctx) {
	ASTNode *program = ast_new_node(AST_PROGRAM);
	root = program;

	for (auto t : ctx->typeDeclaration()) {
        if (ASTNode *n = to_node(visit(t)))
	        ast_add_child(program, n);
	}

	return program;
}

std::any ASTBuilder::visitClassDeclaration(JavaParser::ClassDeclarationContext *ctx) {
	ASTNode *cls = ast_new_node(AST_CLASS_DECL);
	cls->value.str = strdup(ctx->Identifier()->getText().c_str());

	for (auto m : ctx->classBody()->classBodyDeclaration()) {
        if (ASTNode *n = to_node(visit(m)))
	        ast_add_child(cls, n);
	}

	return cls;
}

std::any ASTBuilder::visitMethodDeclaration(JavaParser::MethodDeclarationContext *ctx) {
	ASTNode *m = ast_new_node(AST_METHOD_DECL);

	if (ctx->Identifier())
		m->value.str = strdup(ctx->Identifier()->getText().c_str());

	if (ctx->methodBody() && ctx->methodBody()->block()) {
        if (ASTNode *body = to_node(visit(ctx->methodBody()->block())))
	        ast_add_child(m, body);
	}

	return m;
}

std::any ASTBuilder::visitBlock(JavaParser::BlockContext *ctx) {
	ASTNode *b = ast_new_node(AST_BLOCK);

    b->scope_level = current_scope++;

	for (auto s : ctx->blockStatement()) {
	    if (ASTNode *n = to_node(visit(s)))
	        ast_add_child(b, n);
	}

    current_scope--;

	return b;
}

std::any ASTBuilder::visitStatement(JavaParser::StatementContext *ctx) {
    if (ctx->block())
        return visit(ctx->block());

    if (ctx->statementExpression())
        return visit(ctx->statementExpression());

    if (ctx->RETURN()) {
        ASTNode *r = ast_new_node(AST_RETURN);

        if (!ctx->expression().empty()) {
            if (ASTNode *expr = to_node(visit(ctx->expression()[0])))
                ast_add_child(r, expr);
        }
        return r;
    }

    return nullptr;
}

std::any ASTBuilder::visitStatementExpression(JavaParser::StatementExpressionContext *ctx)
{
    return visit(ctx->expression());
}

std::any ASTBuilder::visitBlockStatement(JavaParser::BlockStatementContext *ctx) {
    if (ctx->localVariableDeclarationStatement())
        return visit(ctx->localVariableDeclarationStatement());

    if (ctx->statement())
        return visit(ctx->statement());

    return nullptr;
}

std::any ASTBuilder::visitExpression(JavaParser::ExpressionContext *ctx) {
    return visit(ctx->assignmentExpression());
}

std::any ASTBuilder::visitExpressionName(JavaParser::ExpressionNameContext *ctx)
{
    return ast_new_ident(ctx->getText().c_str());
}

std::any ASTBuilder::visitLocalVariableDeclarationStatement(JavaParser::LocalVariableDeclarationStatementContext *ctx)
{
    return visit(ctx->localVariableDeclaration());
}

std::any ASTBuilder::visitLocalVariableDeclaration(JavaParser::LocalVariableDeclarationContext *ctx) {
    ASTNode *last = nullptr;

    for (auto v : ctx->variableDeclarators()->variableDeclarator())
    {
        if (ASTNode *n = to_node(visit(v)))
            last = n;
    }

    return last;
}

std::any ASTBuilder::visitAdditiveExpression(JavaParser::AdditiveExpressionContext *ctx) {
    if (ctx->children.size() == 1)
        return visit(ctx->children[0]);

    ASTNode *node = ast_new_op(ctx->children[1]->getText().c_str());

    ast_add_child(node, to_node(visit(ctx->children[0])));
    ast_add_child(node, to_node(visit(ctx->children[2])));

    return node;
}

std::any ASTBuilder::visitMultiplicativeExpression(JavaParser::MultiplicativeExpressionContext *ctx) {
    if (ctx->children.size() == 1)
        return visit(ctx->children[0]);

    ASTNode *node = ast_new_op(ctx->children[1]->getText().c_str());

    ast_add_child(node, to_node(visit(ctx->children[0])));
    ast_add_child(node, to_node(visit(ctx->children[2])));

    return node;
}

std::any ASTBuilder::visitVariableDeclarator(JavaParser::VariableDeclaratorContext *ctx) {
    ASTNode *n = ast_new_node(AST_VAR_DECL);

    n->value.str = strdup(ctx->variableDeclaratorId()->getText().c_str());

    if (ctx->variableInitializer())
    {
        if (ASTNode *init = to_node(visit(ctx->variableInitializer())))
            ast_add_child(n, init);
    }

    return n;
}

std::any ASTBuilder::visitAssignmentExpression(JavaParser::AssignmentExpressionContext *ctx) {
    if (!ctx->assignmentOperator())
        return visit(ctx->conditionalExpression());

    ASTNode *node = ast_new_node(AST_BINARY_OP);

    node->value.op = strdup(ctx->assignmentOperator()->getText().c_str());

    ASTNode *lhs = to_node(visit(ctx->leftHandSide()));
    ASTNode *rhs = to_node(visit(ctx->expression()));

    if (lhs) ast_add_child(node, lhs);
    if (rhs) ast_add_child(node, rhs);

    return node;
}

std::any ASTBuilder::visitLiteral(JavaParser::LiteralContext *ctx) {
    if (ctx->IntegerLiteral())
        return ast_new_int(std::stoi(ctx->getText()));

    return nullptr;
}

std::any ASTBuilder::visitPrimary( JavaParser::PrimaryContext *ctx )
{
    if (ctx->expressionName())
        return ast_new_ident(ctx->getText().c_str());

    if (ctx->literal())
        return visit(ctx->literal());

    return visitChildren(ctx);
}
