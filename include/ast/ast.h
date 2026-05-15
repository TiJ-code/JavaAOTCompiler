#pragma once

#ifndef _AST_H
#define _AST_H

#include <stddef.h>
#include <stdint.h>

#include "semantic/type.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef enum {
		AST_PROGRAM,

		AST_CLASS_DECL,
		AST_METHOD_DECL,
		AST_BLOCK,
		AST_RETURN,

		AST_VAR_DECL,

		AST_LITERAL,
		AST_IDENTIFIER,
		AST_BINARY_OP,
		AST_UNARY_OP
	} ASTType;

	typedef struct ASTNode ASTNode;

	typedef struct ASTList {
		ASTNode **items;
		size_t count;
		size_t capacity;
	} ASTList;

	typedef union ASTValue
	{
		int32_t int_value;
		char *str;
		char *op;
	} ASTValue;

	struct ASTNode {
		ASTType type;

		ASTValue value;

		ASTList *children;

		int32_t scope_level;

		Type *resolved_type;

		char *decl_type;
	};

	ASTNode *ast_new_node(ASTType type);

	ASTNode *ast_new_int(int32_t value);
	ASTNode *ast_new_ident(const char *name);
	ASTNode *ast_new_op(const char *op);
	ASTNode *ast_new_unary_op(const char *op);

	void ast_add_child(ASTNode *parent, ASTNode *child);

	void ast_print(ASTNode *node, int32_t indent);

#ifdef __cplusplus
}
#endif

#endif
