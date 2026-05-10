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
	AST_BLOCK,
	AST_RETURN,

	AST_LITERAL,
	AST_IDENTIFIER,
	AST_BINARY_OP
} ASTType;

typedef struct ASTNode ASTNode;

typedef struct ASTList {
	ASTNode** items;
	size_t count;
	size_t capacity;
} ASTList;

struct ASTNode {
	ASTType type;
	const char *name;

	int32_t int_value;

	struct ASTNode *left;
	struct ASTNode *right;

    ASTList *children;
};

ASTNode *ast_create(ASTType kind);
void ast_add_child(ASTNode *parent, ASTNode *child);

ASTNode *ast_int(int32_t value);
ASTNode *ast_identifier(const char *name);

void ast_print(ASTNode *node, int32_t indent);

#ifdef __cplusplus
}
#endif

#endif
