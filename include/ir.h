#ifndef IR_H
#define IR_H

#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

typedef enum {
	IR_CONST,
	IR_ADD,
	IR_RETURN
} IRType;

typedef enum {
	OPERAND_CONST,
	OPERAND_TEMP
} OperandKind;

typedef struct {
	OperandKind kind;
	union {
		int32_t value;
		int32_t temp_id;
	} as;
} IROperand;

typedef struct {
	IRType type;

	IROperand a;
	IROperand b;
	
	int32_t destination;
} IRInstruction;

typedef struct {
	IRInstruction instructions[32];
	size_t count;
} IRProgram;

IROperand ir_const(int32_t value);
IROperand ir_temp(int32_t id);

bool ir_validate(IRProgram *program);

void ir_build_example(IRProgram *program);

#endif
