#include "semantic/analyzer.h"
#include "semantic/symbol_table.h"
#include "semantic/type.h"
#include <stdio.h>
#include <string.h>

#define semprint(fmt, ...)                                           \
	do {                                                         \
		printf("[Semantic Error] " fmt "\n", ##__VA_ARGS__); \
	} while (0);

static Type *infer_expression_type(ASTNode *node, SymbolTable *table)
{
	if (!node)
		return &TYPE_UNKNOWN_OBJ;

	switch (node->type)
	{
		case AST_LITERAL:
			node->resolved_type = &TYPE_INT_OBJ;
			return &TYPE_INT_OBJ;

		case AST_IDENTIFIER:
		{
			if (symbol_table_exists(table, node->value.str))
				return &TYPE_INT_OBJ;
			return &TYPE_UNKNOWN_OBJ;
		}

		case AST_BINARY_OP:
		{
			ASTNode *lhs = node->children->items[0];
			ASTNode *rhs = node->children->items[1];

			Type *l = infer_expression_type(lhs, table);
			Type *r = infer_expression_type(rhs, table);

			if (l == &TYPE_INT_OBJ && r == &TYPE_INT_OBJ)
			{
				node->resolved_type = &TYPE_INT_OBJ;
				return &TYPE_INT_OBJ;
			}

			node->resolved_type = &TYPE_UNKNOWN_OBJ;
			return &TYPE_UNKNOWN_OBJ;
		}

		default:
			return &TYPE_UNKNOWN_OBJ;
	}
}

static void analyze_node(ASTNode *node, SymbolTable *table);

static void analyze_block(ASTNode *node, SymbolTable *table) {
	symbol_table_push_scope(table);

	for (size_t i = 0; i < node->children->count; i++) {
		analyze_node(node->children->items[i], table);
	}

	symbol_table_pop_scope(table);
}

static void analyze_var_decl(ASTNode *node, SymbolTable *table) {
	const char *name = node->value.str;

	Type *type = &TYPE_UNKNOWN_OBJ;

	if (node->decl_type != NULL) {
		if (strcmp(node->decl_type, "int") == 0) {
			type = &TYPE_INT_OBJ;
		}
	}

	node->resolved_type = type;
        if (node->children->count > 0) {
                ASTNode *init = node->children->items[0];
          
                Type *expr_type = infer_expression_type(init, table);
          
                if (expr_type != type) {
                  semprint("Type mismatch in initializer for %s", name);
                }
        }

	if (!symbol_table_define(table, name, type)) {
		semprint("Duplicate variable: %s", name);
	}

	for (size_t i = 0; i < node->children->count; i++) {
		analyze_node(node->children->items[i], table);
	}
}

static void analyze_ident(ASTNode *node, SymbolTable *table) {
	const char *name = node->value.str;

	if (!symbol_table_exists(table, name)) {
		semprint("Undefined variable: %s", name);
	}
}

static void analyze_node(ASTNode *node, SymbolTable *table) {
	if (!node) return;

	switch (node->type) {
		case AST_BLOCK:
			analyze_block(node, table);
			break;

		case AST_VAR_DECL:
			analyze_var_decl(node, table);
			break;

		case AST_IDENTIFIER:
			analyze_ident(node, table);
			break;

		case AST_BINARY_OP:
			infer_expression_type(node, table);
			break;

		case AST_LITERAL:
			infer_expression_type(node, table);
			break;

		default:
			for (size_t i = 0; i < node->children->count; i++) {
				analyze_node(node->children->items[i], table);
			}
			break;
	}
}

void semantic_analyze(ASTNode *root, SymbolTable *table) {
	symbol_table_push_scope(table);
	analyze_node(root, table);
	symbol_table_pop_scope(table);

	printf("Semantic analysis complete\n");
}
