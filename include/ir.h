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

typedef struct {
	bool is_const;
	int32_t value;
	int32_t temp_id;
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

void ir_build_example(IRProgram *program);

#endif
