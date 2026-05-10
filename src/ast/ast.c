#include "ast/ast.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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

ASTNode *ast_create(ASTType type) {
	ASTNode *n = (ASTNode *)malloc(sizeof(ASTNode));

	memset(n, 0, sizeof(ASTNode));

	n->type = type;

    n->children = (ASTList *)malloc(sizeof(ASTList));
	list_init(n->children);

	return n;
}

ASTNode *ast_var_decl(const char *name, ASTNode *value) {
	ASTNode *n = ast_create(AST_VAR_DECL);
	n->name    = strdup(name);
	n->value   = value;
	return n;
}

void ast_add_child(ASTNode *parent, ASTNode *child) {
    list_add(parent->children, child);
}

ASTNode *ast_int(int32_t value) {
	ASTNode *n   = ast_create(AST_LITERAL);
	n->int_value = value;
	return n;
}

ASTNode *ast_identifier(const char *name) {
	ASTNode *n = ast_create(AST_IDENTIFIER);
	n->name    = strdup(name);
	return n;
}

static inline void print_indent(int32_t indent) {
	for (int32_t i = 0; i < indent; i++)
		printf("    ");
}

void ast_print(ASTNode *node, int32_t indent) {
	if (!node)
		return;

	print_indent(indent);

	switch (node->type) {
		case AST_PROGRAM:     printf("PROGRAM\n");                                 break;
		case AST_CLASS_DECL:  printf("CLASS %s\n", node->name ? node->name : "");  break;
		case AST_METHOD_DECL: printf("METHOD %s\n", node->name ? node->name : ""); break;
		case AST_BLOCK:       printf("BLOCK\n");                                   break;
		case AST_VAR_DECL:    printf("VAR %s\n", node->name ? node->name : "");    break;
		case AST_RETURN:      printf("RETURN\n");                                  break;
		case AST_LITERAL:     printf("LITERAL %d\n", node->int_value);             break;
		case AST_IDENTIFIER:  printf("IDENT %s\n", node->name ? node->name : "");  break;
		case AST_BINARY_OP:   printf("BINOP %s\n", node->name ? node->name : "");  break;
		default:              printf("UNKNOWN\n");                                  break;
	}

	if (node->value) {
		ast_print(node->value, indent+1);
	}

	if (node->left) {
		ast_print(node->left, indent+1);
	}

	if (node->right) {
		ast_print(node->right, indent+1);
	}

	if (node->children) {
		for (size_t i = 0; i < node->children->count; i++) {
			ast_print(node->children->items[i], indent+1);
		}
	}
}
