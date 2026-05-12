#include "ir/ir.h"

#include <stdlib.h>
#include <stdio.h>

static void ensure(IRFunction *f) {
	if (f->count >= f->capacity) {
		f->capacity = f->capacity ? f->capacity * 2 : 8;
		f->instrs = realloc(f->instrs, f->capacity * sizeof(IRInstr));
	}
}

void ir_init(IRFunction *f) {
	f->instrs = NULL;
	f->count = 0;
	f->capacity = 0;
}

void ir_emit(IRFunction *f, IRInstr ins) {
	ensure(f);
	f->instrs[f->count++] = ins;
}

void ir_free(IRFunction *f) {
	free(f->instrs);
	f->instrs = NULL;
	f->count = f->capacity = 0;
}

void ir_print(IRFunction *f) {
	if (!f)
		printf("function-ptr is null\n");

	for (size_t i = 0; i < f->count; i++) {
		IRInstr instr = f->instrs[i];

		ir_instr_to_print(&instr);
	}

	printf("\n");
}

void ir_instr_to_print(IRInstr *inst) {
	if (!inst) {
		printf("null");
		return;
	}

	char result[1024] = "";

	char functype[64];
	switch (inst->type) {
		case IR_CONST:
			sprintf(functype, "CONST");
			break;
		case IR_ADD:
			sprintf(functype, "ADD");
			break;
		case IR_SUB:
			sprintf(functype, "SUB");
			break;
		case IR_MUL:
			sprintf(functype, "MUL");
			break;
		case IR_DIV:
			sprintf(functype, "DIV");
			break;
		case IR_LOAD:
			sprintf(functype, "LOAD");
			break;
		case IR_STORE:
			sprintf(functype, "STORE");
			break;
		case IR_RET:
			sprintf(functype, "RET");
			break;
		default:
			sprintf(functype, "UNKNOWN");
			break;
	}

	strcat(result, functype);
	strcat(result, " ");

		strcat(result, "<");
		if (inst->a.name != NULL) {
			strcat(result, inst->a.name);
			strcat(result, ":");
		}

		char a[20];
		sprintf(a,"%d", inst->a.imm);

		strcat(result, a);
		strcat(result, ">");

	strcat(result, "  ");

		strcat(result, "<");
		if (inst->b.name != NULL) {
			strcat(result, inst->b.name);
			strcat(result, ":");
		}

		char b[20];
		sprintf(b, "%d", inst->b.imm);

		strcat(result, b);
		strcat(result, ">");

	strcat(result, "  ->  ");

		strcat(result, "<");
		if (inst->dst.name != NULL) {
			strcat(result, inst->dst.name);
		} else {
			strcat(result, "null");
		}
		strcat(result, ">");

	printf("%s", result);
}
