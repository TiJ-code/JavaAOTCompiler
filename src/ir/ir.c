#include "ir/ir.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

static void ensure_capacity(IRFunction *f) {
	if (f->count >= f->capacity) {
		f->capacity = f->capacity ? f->capacity * 2 : 8;
		f->instrs = realloc(f->instrs, f->capacity * sizeof(IRInstr));
	}
}

void ir_init(IRFunction *f) {
	f->instrs     = NULL;
	f->count      = 0;
	f->capacity   = 0;
	f->next_temp  = 0;
	f->stack_size = 0;
}

/* ---------------- core ---------------- */

void ir_emit(IRFunction *f, IRInstr ins) {
	ensure_capacity(f);
	f->instrs[f->count++] = ins;
}

int32_t ir_new_temp(IRFunction *f) {
	return f->next_temp++;
}

/* ---------------- CONST ---------------- */

int32_t ir_emit_const(IRFunction *f, int32_t value) {
	int32_t t = ir_new_temp(f);

	IRInstr ins = {
		.type = IR_CONST,
		.dst  = t,
		.cst  = { .imm = value }
	};

	ir_emit(f, ins);
	return t;
}

/* ---------------- BINOP ---------------- */

int32_t ir_emit_binop(IRFunction *f, BinOp op, int32_t a, int32_t b) {
	int32_t t = ir_new_temp(f);

	IRInstr ins = {
		.type  = IR_BINOP,
		.dst   = t,
		.binop = {
			.op  = op,
			.lhs = a,
			.rhs = b
		}
	};

	ir_emit(f, ins);
	return t;
}

/* ---------------- UNARY ---------------- */

int32_t ir_emit_unary(IRFunction *f, UnaryOp op, int32_t src) {
	int32_t t = ir_new_temp(f);

	IRInstr ins = {
		.type  = IR_UNARY,
		.dst   = t,
		.unary = {
			.op  = op,
			.src = src
		}
	};

	ir_emit(f, ins);
	return t;
}

/* ---------------- LOAD ---------------- */

int32_t ir_emit_load(IRFunction *f, Symbol *sym) {
	int32_t t = ir_new_temp(f);

	IRInstr ins = {
		.type = IR_LOAD,
		.dst  = t,
		.mem  = {
			.src          = t,
			.stack_offset = sym->stack_offset,
			.name         = strdup(sym->name)
		}
	};

	ir_emit(f, ins);
	return t;
}

/* ---------------- STORE ---------------- */

void ir_emit_store(IRFunction *f, Symbol *sym, int32_t src) {
	IRInstr ins = {
		.type = IR_STORE,
		.mem  = {
			.src          = src,
			.stack_offset = sym->stack_offset,
			.name         = strdup(sym->name)
		}
	};

	ir_emit(f, ins);
}

/* ---------------- RET ---------------- */

void ir_emit_ret(IRFunction *f, int32_t value) {
	IRInstr ins = {
		.type = IR_RET,
		.ret  = { .value = value }
	};

	ir_emit(f, ins);
}

/* ---------------- PRINT ---------------- */

static void ir_print_instr(IRInstr *ins) {
	switch (ins->type) {

		case IR_CONST:
			printf("CONST t%d <- %d", ins->dst, ins->cst.imm);
			break;

		case IR_BINOP:
			printf("BINOP t%d <- t%d op t%d",
			       ins->dst,
			       ins->binop.lhs,
			       ins->binop.rhs);
			break;

		case IR_UNARY:
			printf("UNARY t%d <- op t%d",
			       ins->dst,
			       ins->unary.src);
			break;

		case IR_LOAD:
			printf("LOAD t%d <- [rbp-%d] (%s)",
			       ins->dst,
			       ins->mem.stack_offset,
			       ins->mem.name ? ins->mem.name : "?");
			break;

		case IR_STORE:
			printf("STORE [rbp-%d] <- t%d (%s)",
			       ins->mem.stack_offset,
			       ins->mem.src,
			       ins->mem.name ? ins->mem.name : "?");
			break;

		case IR_RET:
			printf("RET t%d", ins->ret.value);
			break;

		default:
			printf("UNKNOWN");
			break;
	}

	printf("\n");
}

void ir_print(IRFunction *f) {
	if (!f) {
		printf("IRFunction is null\n");
		return;
	}

	printf("\n === IR DUMP === \n");

	for (size_t i = 0; i < f->count; i++) {
		printf("%04zu  ", i);
		ir_print_instr(&f->instrs[i]);
	}

	printf(" ============== \n");
}

/* ---------------- FREE ---------------- */

void ir_free(IRFunction *f) {
	if (!f) return;

	for (size_t i = 0; i < f->count; i++) {
		IRInstr *ins = &f->instrs[i];

		if ((ins->type == IR_LOAD || ins->type == IR_STORE) && ins->mem.name) {
			free(ins->mem.name);
			ins->mem.name = NULL;
		}
	}

	free(f->instrs);

	f->instrs     = NULL;
	f->count      = 0;
	f->capacity   = 0;
	f->next_temp  = 0;
	f->stack_size = 0;
}
