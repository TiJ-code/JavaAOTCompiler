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
		case 4: return "%r14";
		case 5: return "%r15";
		default: return "%r10";
	}
}

static const char *binop_op(IRInstr *ins) {
	switch (ins->binop.op) {
		case OP_ADD: return "add";
		case OP_SUB: return "sub";
		case OP_MUL: return "mul";
		case OP_DIV: return "div";
		case OP_MOD: return "mod";
		case OP_AND: return "and";
		case OP_OR:  return "or";
		case OP_XOR: return "xor";
	}
	return NULL;
}

static int32_t align16(int32_t size) {
	return ((size + 15) / 16) * 16;
}

void x86_generate(IRFunction *f, FILE *out) {
	int32_t stack_size = align16(f->stack_size);

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
				emit(out, 1, "mov %d, %s", ins->cst.imm, temp_reg(ins->dst));
				break;

			case IR_BINOP: {
				const char *op = binop_op(ins);

				emit(out, 1, "mov %s, %s", temp_reg(ins->binop.lhs), temp_reg(ins->dst));

				if (ins->binop.op == OP_DIV || ins->binop.op == OP_MOD) {
					emit(out, 1, "mov %s, %%rax", temp_reg(ins->binop.lhs));
					emit(out, 1, "cqo");
					emit(out, 1, "idiv %s", temp_reg(ins->binop.rhs));

					if (ins->binop.op == OP_DIV)
						emit(out, 1, "mov %%rax, %s", temp_reg(ins->dst));
					else
						emit(out, 1, "mov %%rdx, %s", temp_reg(ins->dst));
				} else {
					emit(out, 1, "%s %s, %s", op, temp_reg(ins->binop.rhs), temp_reg(ins->dst));
				}
				break;
			}

			case IR_UNARY:
				emit(out, 1, "mov %s, %s", temp_reg(ins->unary.src), temp_reg(ins->dst));

				if (ins->unary.op == UOP_NEG)
					emit(out, 1, "neg %s", temp_reg(ins->dst));
				else if (ins->unary.op == UOP_NOT)
					emit(out, 1, "not %s", temp_reg(ins->dst));

				break;

			case IR_LOAD:
				emit(out, 1, "mov -%d(%%rbp), %s", ins->mem.stack_offset, temp_reg(ins->dst));
				break;

			case IR_STORE:
				emit(out, 1, "mov %s, -%d(%%rbp)", temp_reg(ins->mem.src), ins->mem.stack_offset);
				break;

			case IR_RET:
				emit(out, 1, "mov %s, %%rax", temp_reg(ins->ret.value));
				emit(out, 1, "leave");
				emit(out, 1, "ret");
				break;

			default:
				fprintf(stderr, "Unknown IR type: %d\n", ins->type);
				break;
		}
	}
}
