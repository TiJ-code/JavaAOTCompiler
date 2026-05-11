#include "ast/ast.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void list_add(ASTList *list, ASTNode *node) {
	if (list->count >= list->capacity) {
		list->capacity *= 2;
		list->items     = realloc(list->items, sizeof(ASTNode *) * list->capacity);
	}
	list->items[list->count++] = node;
}

ASTNode *ast_new_node(ASTType type)
{
    ASTNode *n = calloc(1, sizeof(ASTNode));
    n->type = type;

    n->children           = malloc(sizeof(ASTList));
    n->children->count    = 0;
    n->children->capacity = 4;
    n->children->items    = malloc(sizeof(ASTNode*) * 4);

    n->scope_level = 0;

		n->resolved_type = NULL;
		n->decl_type     = NULL;

    return n;
}

ASTNode *ast_new_int(int32_t value) {
    ASTNode *n = ast_new_node(AST_LITERAL);
    n->value.int_value = value;
    return n;
}

ASTNode *ast_new_ident(const char *name) {
    ASTNode *n = ast_new_node(AST_IDENTIFIER);
    n->value.str = strdup(name);
    return n;
}

ASTNode *ast_new_op(const char *op) {
    ASTNode *n = ast_new_node(AST_BINARY_OP);
    n->value.op = strdup(op);
    return n;
}

void ast_add_child(ASTNode *parent, ASTNode *child) {
    if (!parent || !child) return;
    list_add(parent->children, child);
}

void ast_print(ASTNode *node, int indent) {
    if (!node) return;

    for (int i = 0; i < indent; i++)
        printf("    ");

    switch (node->type) {
        case AST_PROGRAM:     printf("PROGRAM\n"); break;
        case AST_CLASS_DECL:  printf("CLASS %s\n", node->value.str ? node->value.str : ""); break;
        case AST_METHOD_DECL: printf("METHOD %s\n", node->value.str ? node->value.str : ""); break;
        case AST_BLOCK:       printf("BLOCK (scope=%d)\n", node->scope_level); break;
				case AST_VAR_DECL:
					printf("VAR %s : %s\n", 
							node->value.str ? node->value.str : "",
							node->decl_type ? node->decl_type : "unknown"
					); 
					break;
        case AST_RETURN:      printf("RETURN\n"); break;
        case AST_LITERAL:
					printf("LITERAL %d <%s>\n", 
							node->value.int_value,
							type_name(node->resolved_type)
					); 
					break;
        case AST_IDENTIFIER:  printf("IDENT %s\n", node->value.str ? node->value.str : ""); break;
				case AST_BINARY_OP:
					printf("BINOP %s <%s>\n",
							node->value.op ? node->value.op : "",
							type_name(node->resolved_type)
					); 
					break;
        default:              printf("UNKNOWN\n"); break;
    }

    for (size_t i = 0; i < node->children->count; i++) {
        ast_print(node->children->items[i], indent + 1);
    }
}
