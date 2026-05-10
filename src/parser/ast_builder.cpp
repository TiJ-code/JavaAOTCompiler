#include "parser/parser.h"

#include "antlr4-runtime.h"
#include "JavaLexer.h"
#include "JavaParser.h"

#include "parser/ast_builder.h"

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
		ASTNode *child = std::any_cast<ASTNode *>(visit(t));
		if (child)
			ast_add_child(program, child);
	}

	return program;
}

std::any ASTBuilder::visitClassDeclaration(JavaParser::ClassDeclarationContext *ctx) {
	ASTNode *cls = ast_create(AST_CLASS_DECL);
	cls->name = strdup(ctx->Identifier()->getText().c_str());

	for (auto m : ctx->classBody()->classBodyDeclaration()) {
		ASTNode *child = std::any_cast<ASTNode *>(visit(m));
		if (child)
			ast_add_child(cls, child);
	}

	return cls;
}

std::any ASTBuilder::visitMethodDeclaration(JavaParser::MethodDeclarationContext *ctx) {
	ASTNode *m = ast_create(AST_METHOD_DECL);
	m->name = strdup(ctx->Identifier()->getText().c_str());
	
	if (ctx->methodBody()) {
		m->left = std::any_cast<ASTNode *>(visit(ctx->methodBody()));
	}

	return m;
}

std::any ASTBuilder::visitBlock(JavaParser::BlockContext *ctx) {
    ASTNode *b = ast_create(AST_BLOCK);

    for (auto s : ctx->blockStatement()) {
        ASTNode *child = std::any_cast<ASTNode *>(visit(s));
        if (child) 
		ast_add_child(b, child);
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
	    auto v = visit(c);
	    if (v.has_value()) {
		    try {
			    last = std::any_cast<ASTNode *>(v);
		    } catch (...) {}
	    }
    }

    return last;
}

std::any ASTBuilder::visitExpression(JavaParser::ExpressionContext *ctx) {
	for (auto c : ctx->children) {
		auto v = visit(c);
		if (v.has_value()) {
			return v;
		}
	}

	return nullptr;
}

std::any ASTBuilder::visitLiteral(JavaParser::LiteralContext *ctx) {
    if (ctx->IntegerLiteral()) {
        return ast_int(std::stoi(ctx->getText()));
    }

    return nullptr;
}
