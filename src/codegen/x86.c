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

static const char *temp_reg(int32_t temp) {
	switch (temp % 4) {
		case 0: return "%r10";
		case 1: return "%r11";
		case 2: return "%r12";
		case 3: return "%r13";
		default: return "%r10";
	}
}

static int32_t aligned_stack_size(int32_t size) {
	return ((size + 15) / 16) * 16;
}

void x86_generate(IRFunction *f, FILE *out) {
	int32_t stack_size = aligned_stack_size(f->stack_size);

	emit(out, 0, ".global main");
	emit(out, 0, "main:");

	emit(out, 1, "push %%rbp");
	emit(out, 1, "mov %%rsp, %%rbp");

	if (stack_size > 0) {
		emit(out, 1, "sub $%d, %%rsp", stack_size);
	}

	emit(out, 0, "");

	for (size_t i = 0; i < f->count; i++) {
		IRInstr *ins = &f->instrs[i];

		switch (ins->type) {
			case IR_CONST:
				emit(out, 1, "mov $%d, %s", ins->imm, temp_reg(ins->dst));
				break;

			case IR_LOAD:
				emit(out, 1, "mov -%d(%%rbp), %s",
						 ins->stack_offset, temp_reg(ins->dst));
				break;

			case IR_STORE:
				emit(out, 1, "mov %s, -%d(%%rbp)",
						 temp_reg(ins->src1), ins->stack_offset);
				break;

			case IR_ADD:
				emit(out, 1, "mov %s, %s", temp_reg(ins->src1), temp_reg(ins->dst));
				emit(out, 1, "add %s, %s", temp_reg(ins->src2), temp_reg(ins->dst));
				break;

			case IR_SUB:
				emit(out, 1, "mov %s, %s", temp_reg(ins->src1), temp_reg(ins->dst));
				emit(out, 1, "sub %s, %s", temp_reg(ins->src2), temp_reg(ins->dst));
				break;

			case IR_MUL:
				emit(out, 1, "mov %s, %s", temp_reg(ins->src1), temp_reg(ins->dst));
				emit(out, 1, "imul %s, %s", temp_reg(ins->src2), temp_reg(ins->dst));
				break;

			case IR_DIV:
				// x86 idivision uses rax/rdx
				emit(out, 1, "mov %s, %%rax", temp_reg(ins->src1));
				emit(out, 1, "cqo");
				emit(out, 1, "idiv %s", temp_reg(ins->src2));
				emit(out, 1, "mov %%rax, %s", temp_reg(ins->dst));
				break;

			case IR_MOD:
				// x86 idivision uses rax/rdx and outputs remainder in rdx
				emit(out, 1, "mov %s, %%rax", temp_reg(ins->src1));
				emit(out, 1, "cqo");
				emit(out, 1, "idiv %s", temp_reg(ins->src2));
				emit(out, 1, "mov %%rdx, %s", temp_reg(ins->dst));
				break;

			case IR_AND:
				emit(out, 1, "mov %s, %s", temp_reg(ins->src1), temp_reg(ins->dst));
				emit(out, 1, "and %s, %s", temp_reg(ins->src2), temp_reg(ins->dst));
				break;

			case IR_OR:
				emit(out, 1, "mov %s, %s", temp_reg(ins->src1), temp_reg(ins->dst));
				emit(out, 1, "or %s, %s", temp_reg(ins->src2), temp_reg(ins->dst));
				break;

			case IR_RET:
				emit(out, 1, "mov %s, %%rax", temp_reg(ins->src1));
				emit(out, 1, "leave");
				emit(out, 1, "ret");
				break;

			default:
				break;
		}
	}
}
