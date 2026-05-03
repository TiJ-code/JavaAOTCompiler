#include "ir.h"

void ir_build_example(IRProgram *program) {
	program->count = 0;

	// t1 = const 60
	program->instructions[program->count++] = (IRInstruction){
		.type = IR_CONST,
		.a = (IROperand){
			.is_const = true,
			.value = 60
		},
		.destination = 2
	};

	// t2 = const 5
	program->instructions[program->count++] = (IRInstruction){
		.type = IR_CONST,
		.a = (IROperand){
			.is_const = true,
			.value = 5
		},
		.destination = 2
	};

	// t3 = add t1, t2
	program->instructions[program->count++] = (IRInstruction){
		.type = IR_ADD,
		.a = (IROperand){
			.is_const = false,
			.temp_id = 1
		},
		.b = (IROperand){
			.is_const = false,
			.temp_id = 2
		},
		.destination = 3
	};

	program->instructions[program->count++] = (IRInstruction){
		.type = IR_RETURN,
		.a = (IROperand){
			.is_const = false,
			.temp_id = 3
		},
		.destination = 0
	};
}
