#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "semantic/symbol_table.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_AND,
	OP_OR,
	OP_XOR
} BinOp;

typedef enum {
	UOP_NEG,
	UOP_NOT
} UnaryOp;

typedef enum {
	IR_CONST,
	IR_BINOP,
	IR_UNARY,
	IR_LOAD,
	IR_STORE,
	IR_RET
} IRType;

typedef struct IRInstr {
	IRType type;

	int32_t dst;

	union {
		struct {
			int32_t imm;
		} cst;

		struct {
			BinOp op;
			int32_t lhs;
			int32_t rhs;
		} binop;

		struct {
			UnaryOp op;
			int32_t src;
		} unary;

		struct {
			int32_t src;
			int32_t stack_offset;
			char *name;
		} mem;

		struct {
			int32_t value;
		} ret;
	};
} IRInstr;

typedef struct IRFunction {
	IRInstr *instrs;
	size_t count;
	size_t capacity;

	int32_t next_temp;
	int32_t stack_size;
} IRFunction;

int32_t ir_emit_const(IRFunction *f, int32_t value);
int32_t ir_emit_binop(IRFunction *f, BinOp op, int32_t a, int32_t b);
int32_t ir_emit_unary(IRFunction *f, UnaryOp op, int32_t src);
void    ir_emit_store(IRFunction *f, Symbol *sym, int32_t src);
int32_t ir_emit_load (IRFunction *f, Symbol *sym);
void    ir_emit_ret  (IRFunction *f, int32_t value);

void ir_init(IRFunction *f);
void ir_emit(IRFunction *f, IRInstr ins);
int32_t ir_new_temp(IRFunction *f);

void ir_print(IRFunction *f);
void ir_free(IRFunction *f);

#ifdef __cplusplus
}
#endif
