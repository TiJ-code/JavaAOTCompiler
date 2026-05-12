#pragma once

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	IR_CONST,
	IR_ADD,
	IR_SUB,
	IR_MUL,
	IR_DIV,
	IR_LOAD,
	IR_STORE,
	IR_RET
} IRType;

typedef struct IRValue {
	int32_t imm;
	char *name;
} IRValue;

typedef struct IRInstr {
	IRType type;

	IRValue a;
	IRValue b;
	IRValue dst;
} IRInstr;

typedef struct IRFunction {
	IRInstr *instrs;
	size_t count;
	size_t capacity;
} IRFunction;

void ir_init(IRFunction *f);
void ir_emit(IRFunction *f, IRInstr ins);
void ir_free(IRFunction *f);

void ir_print(IRFunction *f);

void ir_instr_to_print(IRInstr *inst);
#ifdef __cplusplus
}
#endif
