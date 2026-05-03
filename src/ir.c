#include "ir.h"
#include "stdio.h"
#include "stdbool.h"

#define MIN_TEMP_ID 1
#define MAX_TEMPS 32

static bool defined[MAX_TEMPS];

IROperand ir_const(int32_t value) {
	return (IROperand){
		.kind = OPERAND_CONST,
		.as.value = value
	};
}

IROperand ir_temp(int32_t id) {
	return (IROperand){
		.kind = OPERAND_TEMP,
		.as.temp_id = id
	};
}

bool ir_validate(IRProgram *program) {
	for (size_t i = 0; i < MAX_TEMPS; i++) {
		defined[i] = false;
	}

	for (size_t i = 0; i < program->count; i++) {
		IRInstruction *ins = &program->instructions[i];
          
                switch (ins->type) {
                    case IR_CONST:
                        if (ins->a.kind != OPERAND_CONST) {
                            fprintf(stderr, "IR ERROR: CONST expects immediate\n");
                            return false;
                        }
                        break;
                  
                    case IR_ADD:
                        if (ins->a.kind != OPERAND_TEMP || ins->b.kind != OPERAND_TEMP) {
                            fprintf(stderr, "IR ERROR: ADD expects temps\n");
                            return false;
                        }
                        break;
                  
                    case IR_RETURN:
                        if (ins->a.kind != OPERAND_TEMP) {
                          fprintf("IR ERROR: RETURN expects temp\n");
                          return false;
                        }
                        break;
                } 

		if (ins->a.kind == OPERAND_TEMP) {
			int32_t t = ins->a.as.temp_id;
			if (t < MIN_TEMP_ID || t >= MAX_TEMPS || !defined[t]) {
				fprintf(stderr, "IR ERROR: invalid destination t%d\n", ins->destination);
				return false;
			}
		}

		if (ins->b.kind == OPERAND_TEMP) {
			int32_t t = ins->b.as.temp_id;
			if (t < MIN_TEMP_ID || t >= MAX_TEMPS || !defined[t]) {
				fprintf(stderr, "IR ERROR: invalid destination t%d\n", ins->destination);
				return false;
			}
		}

		if (ins->destination < 0 || ins->destination >= MAX_TEMPS) {
			fprintf(stderr, "IR ERROR: invalid destination t%d\n", ins->destination);
			return false;
		}

		if (defined[ins->destination]) {
			fprintf(stderr, "IR ERROR: temp t%d defined twice (SSA violation)\n", ins->destination);
			return false;
		}

		defined[ins->destination] = true;
	}

	return true;
}

void ir_build_example(IRProgram *program) {
	program->count = 0;

	// t1 = const 60
	program->instructions[program->count++] = (IRInstruction){
		.type = IR_CONST,
		.a = ir_const(60),
		.destination = 1
	};

	// t2 = const 5
	program->instructions[program->count++] = (IRInstruction){
		.type = IR_CONST,
		.a = ir_const(5),
		.destination = 2
	};

	// t3 = add t1, t2
	program->instructions[program->count++] = (IRInstruction){
		.type = IR_ADD,
		.a = ir_temp(1),
		.b = ir_temp(2),
		.destination = 3
	};

	program->instructions[program->count++] = (IRInstruction){
		.type = IR_RETURN,
		.a = ir_temp(3),
		.destination = 0
	};
}
