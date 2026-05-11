#include "ir/ir_builder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static inline IRValue make_const(int32_t value) {
	return (IRValue) {
		.imm = value,
		.name = NULL
	};
}

static inline IRValue make_var(const char *name) {
	return (IRValue) {
		.imm = 0,
		.name = strdup(name)
	};
}

static void ir_lower_node(ASTNode *node, IRFunction *f, SymbolTable *table);

static void ir_lower_block(ASTNode *node, IRFunction *f , SymbolTable *table) {
	for (size_t i = 0; i < node->children->count; i++) {
		ir_lower_node(node->children->items[i], f, table);
	}
}

static void ir_lower_node(ASTNode *node, IRFunction *f, SymbolTable *table) {
	if (!node)
		return;

	switch (node->type) {
		case AST_PROGRAM:
		case AST_CLASS_DECL:
		case AST_METHOD_DECL:
			for (size_t i = 0; i < node->children->count; i++) {
				ir_lower_node(node->children->items[i], f, table);
			}
			break;

		case AST_BLOCK:
			ir_lower_block(node, f, table);
			break;

		case AST_RETURN: {
			if (!node->children || node->children->count == 0)
				return;

			IRValue v = ir_lower_expr(node->children->items[0], f, table);

			IRInstr ins = {
				.type = IR_RET,
				.a = v
			};

			ir_emit(f, ins);
			break;
		}

		default:
			break;
	}
}

IRValue ir_lower_expr(ASTNode *node, IRFunction *f, SymbolTable *table) {
	if (!node)
		return make_const(0);

	switch (node->type) {
		case AST_LITERAL:
			printf("LITERAL value debug: %d\n", node->value.int_value);
			return make_const(node->value.int_value);

		case AST_IDENTIFIER:
			return make_var(node->value.str);

		case AST_BINARY_OP: {
			IRValue l = ir_lower_expr(node->children->items[0], f, table);
			IRValue r = ir_lower_expr(node->children->items[1], f, table);

			IRInstr ins;

			if (strcmp(node->value.op, "+") == 0)
				ins.type = IR_ADD;
			else if (strcmp(node->value.op, "-") == 0)
				ins.type = IR_SUB;
			else if (strcmp(node->value.op, "*") == 0)
				ins.type = IR_MUL;
			else
				ins.type = IR_DIV;

			ins.a = l;
			ins.b = r;

			IRValue tmp;
			tmp.name = strdup("name");
			tmp.imm = 0;

			ins.dst = tmp;

			ir_emit(f, ins);

			return tmp;
		}

		default:
			return make_const(0);
	}
}

void ir_lower(ASTNode *root, IRFunction *f, SymbolTable *table) {
	ir_init(f);
	ir_lower_node(root, f, table);
	return;

	for (size_t i = 0; i < root->children->count; i++) {
		ASTNode *n = root->children->items[i];

		if (n->type == AST_RETURN) {
			IRValue v = ir_lower_expr(n->children->items[0], f, table);

			IRInstr ins = {
				.type = IR_RET,
				.a = v
			};

			ir_emit(f, ins);
		}
	}
}
