#include "ir.h"

void ir_build_example(IRProgram *program) {
	static IRInstruction instructions[] = {
		{ IR_CONST, 60 },
		{ IR_CONST,  5 },
		{ IR_ADD,    0 },
		{ IR_RETURN, 0 }
	};

	program->instructions = instructions;
	program->count        = 4;
}
