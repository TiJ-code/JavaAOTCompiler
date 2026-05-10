#pragma once

#ifndef _AST_BUILDER_H
#define _AST_BUILDER_H

#include "JavaParserBaseVisitor.h"
#include "ast/ast.h"
#include "semantic/symbol_table.h"

class ASTBuilder : public JavaParserBaseVisitor {
public:
	ASTNode *root = nullptr;
	SymbolTable symbols;

	std::any visitCompilationUnit(JavaParser::CompilationUnitContext *ctx) override;

	std::any visitClassDeclaration(JavaParser::ClassDeclarationContext *ctx) override;

	std::any visitMethodDeclaration(JavaParser::MethodDeclarationContext *ctx) override;

	std::any visitBlock(JavaParser::BlockContext *ctx) override;

	std::any visitStatement(JavaParser::StatementContext *ctx) override;

    std::any visitBlockStatement(JavaParser::BlockStatementContext *ctx) override;

	std::any visitExpression(JavaParser::ExpressionContext *ctx) override;

    std::any visitLocalVariableDeclarationStatement(JavaParser::LocalVariableDeclarationStatementContext *ctx) override;

	std::any visitLocalVariableDeclaration(JavaParser::LocalVariableDeclarationContext *ctx) override;

	std::any visitAdditiveExpression(JavaParser::AdditiveExpressionContext *ctx) override;

	std::any visitMultiplicativeExpression(JavaParser::MultiplicativeExpressionContext *ctx) override;

	std::any visitAssignmentExpression(JavaParser::AssignmentExpressionContext *ctx) override;

	std::any visitLiteral(JavaParser::LiteralContext *ctx) override;

    std::any visitPrimary(JavaParser::PrimaryContext *ctx) override;
};

#endif
