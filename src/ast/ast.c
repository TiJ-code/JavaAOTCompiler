#include "ast/ast.h"
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>

static void list_init(ASTList *list) {
	list->count    = 0;
	list->capacity = 4;
	list->items    = malloc(sizeof(ASTNode *) * list->capacity);
}

static void list_add(ASTList *list, ASTNode *node) {
	if (list->count >= list->capacity) {
		list->capacity *= 2;
		list->items     = realloc(list->items, sizeof(ASTNode *) * list->capacity);
	}
	list->items[list->count++] = node;
}

ASTNode *ast_create(ASTKind kind) {
	ASTNode *n = (ASTNode *)malloc(sizeof(ASTNode));
	memset(n, 0, sizeof(ASTNode));
	n->kind = kind;
	list_init(&n->children);
	return n;
}

void ast_add_child(ASTNode *parent, ASTNode *child) {
	list_add(&parent->children, child);
}

ASTNode *ast_int(int32_t value) {
	ASTNode *n   = ast_create(AST_LITERAL_INT);
	n->int_value = value;
	return n;
}

ASTNode *ast_identifier(const char *name) {
	ASTNode *n = ast_create(AST_IDENTIFIER);
	n->name    = strdup(name);
	return n;
}
