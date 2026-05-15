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
	IR_LOAD,
	IR_STORE,

	IR_ADD,
	IR_SUB,
	IR_MUL,
	IR_DIV,
	IR_MOD,

	IR_AND,
	IR_OR,
	IR_XOR,
	IR_NOT,
	IR_NEG,

	IR_RET
} IRType;

typedef struct IRInstr {
	IRType type;

	int32_t dst;

	int32_t src1;
	int32_t src2;

	int32_t imm;
	
	char *name;
	int32_t stack_offset;
} IRInstr;

typedef struct IRFunction {
	IRInstr *instrs;
	size_t count;
	size_t capacity;

	int32_t next_temp;
	int32_t stack_size;
} IRFunction;

void ir_init(IRFunction *f);
void ir_emit(IRFunction *f, IRInstr ins);
int32_t ir_new_temp(IRFunction *f);

void ir_print(IRFunction *f);
void ir_free(IRFunction *f);

#ifdef __cplusplus
}
#endif
