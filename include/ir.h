#ifndef IR_H
#define IR_H

typedef enum {
	IR_CONST,
	IR_ADD,
	IR_RETURN
} IRType;

typedef struct {
	IRType type;
	int value;
} IRInstruction;

typedef struct {
	IRInstruction *instructions;
	int count;
} IRProgram;

void ir_build_example(IRProgram *program);

#endif
