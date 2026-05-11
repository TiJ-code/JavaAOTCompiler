#include "codegen/x86.h"
#include <stdarg.h>

static void emit(FILE *out, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	vfprintf(out, fmt, args);
	fprintf(out, "\n");
	va_end(args);
}

void x86_generate(IRFunction *f, FILE *out) {
	emit(out, ".global main");
	emit(out, "main:");

	for (size_t i = 0; i < f->count; i++) {
		IRInstr *ins = &f->instrs[i];

		switch (ins->type) {
			case IR_CONST:
				emit(out, " mov $%d, %%rax", ins->a.imm);
				break;

			case IR_ADD:
				emit(out, " mov $%d, %%rax", ins->a.imm);
				emit(out, " add $%d, %%rax", ins->b.imm);
				break;

			case IR_SUB:
				emit(out, " mov $%d, %%rax", ins->a.imm);
				emit(out, " sub $%d, %%rax", ins->b.imm);
				break;

			case IR_MUL:
				emit(out, " mov $%d, %%rax", ins->a.imm);
				emit(out, " imul $%d, %%rax", ins->b.imm);
				break;

			case IR_RET:
				if (ins->a.name == NULL)
					emit(out, " mov $%d, %%rax", ins->a.imm);
				else {
					emit(out, "   # TODO load variable %s properly", ins->a.name);
					emit(out, " mov $0, %%rax");
				}
				emit(out, " ret");
				break;

			default:
				break;
		}
	}
}
