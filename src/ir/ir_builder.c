#include "ir/ir_builder.h"

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void ir_lower_node(ASTNode *node, IRFunction *f, SymbolTable *table);

/* ---------------- OP conversion ---------------- */

static BinOp to_binop(const char *op) {
	if (!strcmp(op, "+")) return OP_ADD;
	if (!strcmp(op, "-")) return OP_SUB;
	if (!strcmp(op, "*")) return OP_MUL;
	if (!strcmp(op, "/")) return OP_DIV;
	if (!strcmp(op, "%")) return OP_MOD;
	if (!strcmp(op, "&")) return OP_AND;
	if (!strcmp(op, "|")) return OP_OR;
	if (!strcmp(op, "^")) return OP_XOR;

	fprintf(stderr, "unknown binary op: %s\n", op);
	exit(1);
}

static int is_assignment_op(const char *op) {
	return !strcmp(op, "=")  ||
	       !strcmp(op, "+=") ||
	       !strcmp(op, "-=") ||
	       !strcmp(op, "*=") ||
	       !strcmp(op, "/=") ||
	       !strcmp(op, "%=") ||
	       !strcmp(op, "&=") ||
	       !strcmp(op, "|=") ||
	       !strcmp(op, "^=");
}

/* ---------------- block ---------------- */

static void ir_lower_block(ASTNode *node, IRFunction *f, SymbolTable *table) {
	for (size_t i = 0; i < node->children->count; i++) {
		ir_lower_node(node->children->items[i], f, table);
	}
}

/* ---------------- assignment ---------------- */

static void ir_lower_assignment(ASTNode *node, IRFunction *f, SymbolTable *table) {
	ASTNode *lhs = node->children->items[0];
	ASTNode *rhs = node->children->items[1];

	Symbol *sym = symbol_table_lookup(table, lhs->value.str);
	if (!sym) {
		fprintf(stderr, "unknown symbol: %s\n", lhs->value.str);
		exit(1);
	}

	/* x = expr */
	if (!strcmp(node->value.op, "=")) {
		int32_t v = ir_lower_expr(rhs, f, table);
		ir_emit_store(f, sym, v);
		return;
	}

	/* x += expr style */
	int32_t a = ir_emit_load(f, sym);
	int32_t b = ir_lower_expr(rhs, f, table);

	int32_t r = ir_emit_binop(f, to_binop(node->value.op), a, b);

	ir_emit_store(f, sym, r);
}

/* ---------------- var decl ---------------- */

static void ir_lower_var_decl(ASTNode *node, IRFunction *f, SymbolTable *table) {
	if (!node->children || node->children->count == 0)
		return;

	Symbol *sym = symbol_table_lookup(table, node->value.str);
	if (!sym) {
		fprintf(stderr, "unknown symbol: %s\n", node->value.str);
		exit(1);
	}

	int32_t v = ir_lower_expr(node->children->items[0], f, table);
	ir_emit_store(f, sym, v);
}

/* ---------------- node walker ---------------- */

static void ir_lower_node(ASTNode *node, IRFunction *f, SymbolTable *table) {
	if (!node) return;

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

		case AST_VAR_DECL:
			ir_lower_var_decl(node, f, table);
			break;

		case AST_RETURN: {
			int32_t v = ir_lower_expr(node->children->items[0], f, table);
			ir_emit_ret(f, v);
			break;
		}

		case AST_BINARY_OP:
			if (is_assignment_op(node->value.op)) {
				ir_lower_assignment(node, f, table);
			}
			break;

		default:
			break;
	}
}

/* ---------------- expr ---------------- */

int32_t ir_lower_expr(ASTNode *node, IRFunction *f, SymbolTable *table) {
	if (!node) return -1;

	switch (node->type) {

		case AST_LITERAL:
			return ir_emit_const(f, node->value.int_value);

		case AST_IDENTIFIER: {
			Symbol *sym = symbol_table_lookup(table, node->value.str);
			if (!sym) {
				fprintf(stderr, "unknown symbol: %s\n", node->value.str);
				exit(1);
			}
			return ir_emit_load(f, sym);
		}

		case AST_BINARY_OP: {
			if (is_assignment_op(node->value.op)) {
				ir_lower_assignment(node, f, table);

				Symbol *sym = symbol_table_lookup(table, node->children->items[0]->value.str);
				return ir_emit_load(f, sym);
			}

			int32_t a = ir_lower_expr(node->children->items[0], f, table);
			int32_t b = ir_lower_expr(node->children->items[1], f, table);

			return ir_emit_binop(f, to_binop(node->value.op), a, b);
		}

		case AST_UNARY_OP: {
			int32_t v = ir_lower_expr(node->children->items[0], f, table);

			if (!strcmp(node->value.op, "-"))
				return ir_emit_unary(f, UOP_NEG, v);

			if (!strcmp(node->value.op, "~"))
				return ir_emit_unary(f, UOP_NOT, v);

			fprintf(stderr, "unknown unary op: %s\n", node->value.op);
			exit(1);
		}

		default:
			fprintf(stderr, "unsupported AST node\n");
			exit(1);
	}
}

/* ---------------- entry ---------------- */

void ir_lower(ASTNode *root, IRFunction *f, SymbolTable *table) {
	ir_init(f);
	ir_lower_node(root, f, table);
	f->stack_size = table->next_stack_offset;
}
