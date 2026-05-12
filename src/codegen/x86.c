#include "codegen/x86.h"
#include <stdarg.h>

static inline void print_indent(FILE *out, int32_t depth) {
	for (int32_t i = 0; i < depth; i++) {
		fprintf(out, "    ");
	}
}

static void emit(FILE *out, int32_t indent, const char *fmt, ...) {
	print_indent(out, indent);

	va_list args;
	va_start(args, fmt);
	vfprintf(out, fmt, args);
	fprintf(out, "\n");
	va_end(args);
}

void x86_generate(IRFunction *f, FILE *out) {
	emit(out, 0, ".global main");
	emit(out, 0, "main:");

	for (size_t i = 0; i < f->count; i++) {
		IRInstr *ins = &f->instrs[i];

		switch (ins->type) {
			case IR_CONST:
				emit(out, 1, "mov $%d, %%rax", ins->a.imm);
				break;

			case IR_ADD:
				emit(out, 1, "mov $%d, %%rax", ins->a.imm);
				emit(out, 1, "add $%d, %%rax", ins->b.imm);
				break;

			case IR_SUB:
				emit(out, 1, "mov $%d, %%rax", ins->a.imm);
				emit(out, 1, "sub $%d, %%rax", ins->b.imm);
				break;

			case IR_MUL:
				emit(out, 1, "mov $%d, %%rax", ins->a.imm);
				emit(out, 1, "imul $%d, %%rax", ins->b.imm);
				break;

			case IR_RET:
				if (ins->a.name == NULL)
					emit(out, 1, "mov $%d, %%rax", ins->a.imm);
				else {
					emit(out, 1, "    # TODO load variable %s properly", ins->a.name);
					emit(out, 1, "mov $0, %%rax");
				}
				emit(out, 1, "ret");
				break;

			default:
				break;
		}
	}
}
