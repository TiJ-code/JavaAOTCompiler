#include "semantic/analyzer.h"
#include "semantic/symbol_table.h"
#include <stdio.h>

#define semprint(fmt, ...)                               \
	do {                                                   \
		printf("[Semantic Error] " fmt "\n", ##__VA_ARGS__); \
	} while (0);

static SymbolTable table;

static void analyze_node(ASTNode *node, SymbolTable *table);

static void analyze_block(ASTNode *node, SymbolTable *table) {
	symbol_table_push_scope(table);

	for (size_t i = 0; i < node->children->count; i++) {
		analyze_node(node->children->items[i], table);
	}

	symbol_table_pop_scope(table);
}

static void analyze_var_decl(ASTNode *node) {
	const char *name = node->value.str;

	if (!symbol_table_define(&table, name)) {
		semprint("Duplicate variable: %s", name);
	}
}

static void analyze_ident(ASTNode *node) {
	const char *name = node->value.str;

	if (!symbol_table_exists(&table, name)) {
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
			analyze_var_decl(node);
			break;

		case AST_IDENTIFIER:
			analyze_ident(node);
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
