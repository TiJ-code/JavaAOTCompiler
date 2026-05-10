#pragma once

#ifndef _AST_H
#define _AST_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	AST_PROGRAM,

	AST_CLASS_DECL,
	AST_METHOD_DECL,
	AST_PARAM,

	AST_BLOCK,
	AST_RETURN,

	AST_VAR_DECL,

	AST_BINARY_EXPR,
	AST_CALL_EXPR,
	AST_LITERAL_INT,
	AST_IDENTIFIER
} ASTKind;

typedef struct ASTNode ASTNode;

typedef struct ASTList {
	ASTNode** items;
	size_t count;
	size_t capacity;
} ASTList;

struct ASTNode {
	ASTKind kind;

	const char *name;

	ASTList children;

	int32_t int_value;

	ASTNode *left;
	ASTNode *right;
};

ASTNode *ast_create(ASTKind kind);
void ast_add_child(ASTNode *parent, ASTNode *child);

ASTNode *ast_int(int32_t value);
ASTNode *ast_identifier(const char *name);

#ifdef __cplusplus
}
#endif

#endif
