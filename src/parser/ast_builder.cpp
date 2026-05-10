#include "parser/parser.h"

#include "JavaLexer.h"
#include "JavaParser.h"
#include "antlr4-runtime.h"

#include "parser/ast_builder.h"
#include "semantic/symbol_table.h"

#include <any>
#include <fstream>

using namespace antlr4;

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
	ASTNode *program = ast_create(AST_PROGRAM);
	root = program;

	for (auto t : ctx->typeDeclaration()) {
		try {
			auto v = visit(t);

			if (v.has_value()) {
				ASTNode *child = std::any_cast<ASTNode *>(v);
				if (child)
					ast_add_child(program, child);
			}
		} catch (...) {}
	}

	return program;
}

std::any ASTBuilder::visitClassDeclaration(JavaParser::ClassDeclarationContext *ctx) {
	ASTNode *cls = ast_create(AST_CLASS_DECL);
	cls->name = strdup(ctx->Identifier()->getText().c_str());

	for (auto m : ctx->classBody()->classBodyDeclaration()) {
		try {
			auto v = visit(m);

			if (v.has_value()) {
				ASTNode *child = std::any_cast<ASTNode *>(v);
				if (child)
					ast_add_child(cls, child);
			}
		} catch (...) {}
	}

	return cls;
}

std::any ASTBuilder::visitMethodDeclaration(JavaParser::MethodDeclarationContext *ctx) {
	ASTNode *m = ast_create(AST_METHOD_DECL);

	auto id = ctx->Identifier();

	if (id)
		m->name = strdup(id->getText().c_str());

	if (ctx->methodBody() && ctx->methodBody()->block()) {
		m->left = std::any_cast<ASTNode *>(visit(ctx->methodBody()->block()));
	}

	return m;
}

std::any ASTBuilder::visitBlock(JavaParser::BlockContext *ctx) {
	ASTNode *b = ast_create(AST_BLOCK);

	for (auto s : ctx->blockStatement()) {
		try {
			auto v = visit(s);

			if (v.has_value()) {
				ASTNode *child = std::any_cast<ASTNode *>(v);

				if (child)
					ast_add_child(b, child);
			}
		} catch (...) {}
	}

	return b;
}

std::any ASTBuilder::visitStatement(JavaParser::StatementContext *ctx) {
	ASTNode *r = ast_create(AST_RETURN);

	if (ctx->RETURN()) {
		ASTNode *r = ast_create(AST_RETURN);

		if (!ctx->expression().empty()) {
			auto expreCtx = ctx->expression()[0];

			r->left = std::any_cast<ASTNode *>(visit(expreCtx));
		}

		return r;
	}

	ASTNode *last = nullptr;

	for (auto c : ctx->children) {
		if (c) {
			auto v = visit(c);
			if (v.has_value()) {
				return v;
			}
		}
	}

	return last;
}

std::any ASTBuilder::visitBlockStatement( JavaParser::BlockStatementContext *ctx )
{
    for (auto c : ctx->children) {
        if (!c)
            continue;

        auto v = visit(c);

        if (v.has_value())
            return v;
    }

    return nullptr;
}

std::any ASTBuilder::visitExpression(JavaParser::ExpressionContext *ctx) {
    if (!ctx->children.empty())
        return visit(ctx->children[0]);
	return nullptr;
}

std::any ASTBuilder::visitLocalVariableDeclarationStatement(JavaParser::LocalVariableDeclarationStatementContext *ctx)
{
    return visit(ctx->localVariableDeclaration());
}

std::any ASTBuilder::visitLocalVariableDeclaration(JavaParser::LocalVariableDeclarationContext *ctx) {
    ASTNode *last = nullptr;

    for (auto v : ctx->variableDeclarators()->variableDeclarator()) {
        auto result = visit(v);

        if (result.has_value()) {
            last = std::any_cast<ASTNode *>(result);
        }
    }

    return last;
}

std::any ASTBuilder::visitAdditiveExpression(JavaParser::AdditiveExpressionContext *ctx) {
	if (ctx->children.size() == 1)
		return visit(ctx->children[0]);

	ASTNode *node = ast_create(AST_BINARY_OP);

	node->left = std::any_cast<ASTNode *>(visit(ctx->children[0]));

	node->name = strdup(ctx->children[1]->getText().c_str());

	node->right = std::any_cast<ASTNode *>(visit(ctx->children[2]));

	return node;
}

std::any ASTBuilder::visitMultiplicativeExpression(JavaParser::MultiplicativeExpressionContext *ctx) {
	if (ctx->children.size() == 1)
		return visit(ctx->children[0]);

	ASTNode *node = ast_create(AST_BINARY_OP);

	node->left = std::any_cast<ASTNode *>(visit(ctx->children[0]));

	node->name = strdup(ctx->children[1]->getText().c_str());

	node->right = std::any_cast<ASTNode *>(visit(ctx->children[2]));

	return node;

}

std::any ASTBuilder::visitAssignmentExpression(JavaParser::AssignmentExpressionContext *ctx) {
	if (ctx->children.size() == 1)
		return visit(ctx->children[0]);

	ASTNode *node = ast_create(AST_BINARY_OP);

	node->left = std::any_cast<ASTNode *>(visit(ctx->children[0]));
	node->right = std::any_cast<ASTNode *>(visit(ctx->children[2]));

	node->name = strdup(ctx->children[1]->getText().c_str());

	return node;
}

std::any ASTBuilder::visitLiteral(JavaParser::LiteralContext *ctx) {
	if (ctx->IntegerLiteral()) {
		return ast_int(std::stoi(ctx->getText()));
	}

	return nullptr;
}

std::any ASTBuilder::visitPrimary( JavaParser::PrimaryContext *ctx )
{
    if (ctx->expressionName())
    {
        return ast_identifier(ctx->getText().c_str());
    }

    if (ctx->literal())
    {
        return visit(ctx->literal());
    }

    return visitChildren(ctx);
}
