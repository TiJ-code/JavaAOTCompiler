#include "ir/ir.h"

#include <stdlib.h>
#include <stdio.h>

static void ensure_capacity(IRFunction *f) {
	if (f->count >= f->capacity) {
		f->capacity = f->capacity ? f->capacity * 2 : 8;
		f->instrs = realloc(
				f->instrs,
			 	f->capacity * sizeof(IRInstr)
		);
	}
}

void ir_init(IRFunction *f) {
	f->instrs = NULL;

	f->count = 0;
	f->capacity = 0;

	f->next_temp = 0;
	f->stack_size = 0;
}

void ir_emit(IRFunction *f, IRInstr ins) {
	ensure_capacity(f);
	f->instrs[f->count++] = ins;
}

int32_t ir_new_temp(IRFunction *f) {
	return f->next_temp++;
}

void ir_free(IRFunction *f) {
	if (!f)
		return;

	for (size_t i = 0; i < f->count; i++) {
		IRInstr *ins = &f->instrs[i];

		if (ins->name != NULL) {
			free(ins->name);
			ins->name = NULL;
		}
	}

	free(f->instrs);
	f->instrs     = NULL;

	f->count      = 0;
	f->capacity   = 0;

	f->next_temp  = 0;
	f->stack_size = 0;
}

static const char *ir_type_name(IRType type) {
	switch (type) {
		case IR_CONST: return "CONST";
		case IR_LOAD:  return "LOAD";
		case IR_STORE: return "STORE";

		case IR_ADD:   return "ADD";
		case IR_SUB:   return "SUB";
		case IR_MUL:   return "MUL";
		case IR_DIV:   return "DIV";
		case IR_MOD:   return "MOD";

		case IR_RET:   return "RET";
		default:       return "UNKNOWN";
	}
}

static void print_temp(int32_t temp) {
	if (temp >= 0)
		printf("t%d", temp);
	else
		printf("-");
}

static void ir_print_instr(IRInstr *ins) {
	printf("%-8s ", ir_type_name(ins->type));

	switch (ins->type) {
		case IR_CONST:
			print_temp(ins->dst);
			printf(" <- %d", ins->imm);
			break;

		case IR_LOAD:
			print_temp(ins->dst);
			printf(
					" <- [rbp-%d] (%s)",
					ins->stack_offset,
					ins->name ? ins->name : "?"
			);
			break;

		case IR_STORE:
			printf(
					"[rbp-%d] (%s) <- ",
					ins->stack_offset,
					ins->name ? ins->name : "?"
			);
			print_temp(ins->src1);
			break;

		case IR_ADD:
		case IR_SUB:
		case IR_MUL:
		case IR_DIV:
		case IR_MOD:
			print_temp(ins->dst);
			printf(" <- ");
			print_temp(ins->src1);
			printf(" , ");
			print_temp(ins->src2);
			break;

		case IR_RET:
			print_temp(ins->src1);
			break;

		default:
			printf("invalid");
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

