#include "ir/ir_builder.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void ir_lower_node(ASTNode *node, IRFunction *f, SymbolTable *table);

static IRType binary_op_type(const char *op) {
	if (strcmp(op, "+") == 0)
		return IR_ADD;

	if (strcmp(op, "-") == 0)
		return IR_SUB;

	if (strcmp(op, "*") == 0)
		return IR_MUL;

	if (strcmp(op, "/") == 0)
		return IR_DIV;

	if (strcmp(op, "%") == 0)
		return IR_MOD;

	if (strcmp(op, "&") == 0)
		return IR_AND;

	if (strcmp(op, "|") == 0)
		return IR_OR;

	return IR_DIV;
}

static void ir_lower_block(ASTNode *node, IRFunction *f , SymbolTable *table) {
	for (size_t i = 0; i < node->children->count; i++) {
		ir_lower_node(node->children->items[i], f, table);
	}
}

static void emit_store(IRFunction *f, Symbol *sym, int32_t src_temp) {
	IRInstr ins = {
		.type         = IR_STORE,
		.src1         = src_temp,
		.stack_offset = sym->stack_offset,
		.name         = strdup(sym->name)
	};

	ir_emit(f, ins);
}

static int32_t emit_load(IRFunction *f, Symbol *sym) {
	int32_t dst = ir_new_temp(f);

	IRInstr ins = {
		.type         = IR_LOAD,
		.dst          = dst,
		.stack_offset = sym->stack_offset,
		.name         = strdup(sym->name)
	};

	ir_emit(f, ins);

	return dst;
}

static void ir_lower_assignment(ASTNode *node, IRFunction *f, SymbolTable *table) {
	ASTNode *lhs = node->children->items[0];
	ASTNode *rhs = node->children->items[1];

	Symbol *sym = symbol_table_lookup(table, lhs->value.str);

	if (!sym)
		return;

	// x = expr;
	if (strcmp(node->value.op, "=") == 0) {
		int32_t rhs_temp = ir_lower_expr(rhs, f, table);

		emit_store(f, sym, rhs_temp);
		return;
	}

	// x += expr;
	// x -= expr;
	// ...
	int32_t lhs_temp = emit_load(f, sym);
	int32_t rhs_temp = ir_lower_expr(rhs, f, table);

	int32_t result = ir_new_temp(f);

	IRInstr op = {
		.dst  = result,
		.src1 = lhs_temp,
		.src2 = rhs_temp
	};

	if (strcmp(node->value.op, "+=") == 0)
		op.type = IR_ADD;
	else if (strcmp(node->value.op, "-=") == 0)
		op.type = IR_SUB;
	else if (strcmp(node->value.op, "*=") == 0)
		op.type = IR_MUL;
	else if (strcmp(node->value.op, "%=") == 0)
		op.type = IR_MOD;
	else if (strcmp(node->value.op, "&=") == 0)
		op.type = IR_AND;
	else if (strcmp(node->value.op, "|=") == 0)
		op.type = IR_OR;
	else
		op.type = IR_DIV;

	ir_emit(f, op);

	emit_store(f, sym, result);


}

static void ir_lower_var_decl(ASTNode *node, IRFunction *f, SymbolTable *table) {
	if (!node->children || node->children->count == 0)
		return;

	Symbol *sym = symbol_table_lookup(table, node->value.str);

	if (!sym)
		return;

	ASTNode *init = node->children->items[0];

	int32_t value = ir_lower_expr(init, f, table);

	emit_store(f, sym, value);
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

		case AST_VAR_DECL:
			ir_lower_var_decl(node, f, table);
			break;

		case AST_RETURN: {
			if (!node->children || node->children->count == 0)
				return;

			int32_t value = ir_lower_expr(node->children->items[0], f, table);

			IRInstr ins = {
				.type = IR_RET,
				.src1 = value
			};

			ir_emit(f, ins);
			break;
		}

		case AST_BINARY_OP:
			if (strcmp(node->value.op, "=") == 0 ||
					strcmp(node->value.op, "+=") == 0 ||
					strcmp(node->value.op, "-=") == 0 ||
					strcmp(node->value.op, "*=") == 0 ||
					strcmp(node->value.op, "/=") == 0 ||
					strcmp(node->value.op, "%=") == 0 ||
					strcmp(node->value.op, "&=") == 0 ||
					strcmp(node->value.op, "|=") == 0) {
				ir_lower_assignment(node, f, table);
			}
			break;

		default:
			break;
	}
}

int32_t ir_lower_expr(ASTNode *node, IRFunction *f, SymbolTable *table) {
	if (!node)
		return -1;

	switch (node->type) {
		case AST_LITERAL: {
			int32_t dst = ir_new_temp(f);

			IRInstr ins = {
				.type = IR_CONST,
				.dst = dst,
				.imm = node->value.int_value
			};

			ir_emit(f, ins);

			return dst;
		}

		case AST_IDENTIFIER: {
			Symbol *sym = symbol_table_lookup(table, node->value.str);

			if (!sym)
				return -1;

			return emit_load(f, sym);
		}

		case AST_BINARY_OP: {
			if (strcmp(node->value.op, "=") == 0 ||
					strcmp(node->value.op, "+=") == 0 ||
					strcmp(node->value.op, "-=") == 0 ||
					strcmp(node->value.op, "*=") == 0 ||
					strcmp(node->value.op, "/=") == 0 ||
					strcmp(node->value.op, "%=") == 0 ||
					strcmp(node->value.op, "&=") == 0 ||
					strcmp(node->value.op, "|=") == 0) {
				ir_lower_assignment(node, f, table);

				ASTNode *lhs = node->children->items[0];

				Symbol *sym = symbol_table_lookup(
						table, lhs->value.str
				);

				if (!sym)
					return -1;

				return emit_load(f, sym);
			}

			int32_t lhs = ir_lower_expr(
					node->children->items[0],
					f, table
			);

			int32_t rhs = ir_lower_expr(
					node->children->items[1],
					f, table
			);

			int32_t dst = ir_new_temp(f);

			IRInstr ins = {
				.type = binary_op_type(node->value.op),
				.dst = dst,
				.src1 = lhs,
				.src2 = rhs
			};

			ir_emit(f, ins);

			return dst;
		}

		default:
			return -1;
	}
}

void ir_lower(ASTNode *root, IRFunction *f, SymbolTable *table) {
	ir_init(f);

	ir_lower_node(root, f, table);

	f->stack_size = table->next_stack_offset;
}
