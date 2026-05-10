#pragma once

#ifndef _AST_BUILDER_H
#define _AST_BUILDER_H

#include "JavaParserBaseVisitor.h"
#include "ast/ast.h"

class ASTBuilder : public JavaParserBaseVisitor {
public:
	ASTNode *root = nullptr;

	virtual std::any visitCompilationUnit(JavaParser::CompilationUnitContext *ctx) override;

	virtual std::any visitClassDeclaration(JavaParser::ClassDeclarationContext *ctx) override;

	virtual std::any visitMethodDeclaration(JavaParser::MethodDeclarationContext *ctx) override;

	virtual std::any visitBlock(JavaParser::BlockContext *ctx) override;

	virtual std::any visitStatement(JavaParser::StatementContext *ctx) override;

	virtual std::any visitExpression(JavaParser::ExpressionContext *ctx) override;

	virtual std::any visitLiteral(JavaParser::LiteralContext *ctx) override;
};

#endif
