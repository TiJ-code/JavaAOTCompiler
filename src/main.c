#include <stdio.h>
#include <stdint.h>

#include "elf_writer.h"
#include "codegeneration.h"
#include "ir.h"
#include "parser.h"

static inline int32_t reg_code(int32_t temp_id) {
	switch (temp_id) {
		case 1: return 0; // rax
		case 2: return 3; // rbx
		case 3: return 1; // rcx
		case 4: return 2; // rdx
                case 5: return 7; // rdi
		default: return 0;
	}
}

static inline void emit_mov_imm(CodeBuffer *cb, int32_t reg, uint32_t value) {
  cb_emit_u8(cb, 0x48);
  cb_emit_u8(cb, 0xC7);
  cb_emit_u8(cb, 0xC0 | reg);
  cb_emit_u32(cb, value);
}

static inline void emit_mov_reg(CodeBuffer *cb, int32_t dst, int src) {
  cb_emit_u8(cb, 0x48);
  cb_emit_u8(cb, 0x89);
  cb_emit_u8(cb, 0xC0 | (src << 3) | dst);
}

static inline void emit_add_reg(CodeBuffer *cb, int32_t dst, int32_t src) {
  cb_emit_u8(cb, 0x48);
  cb_emit_u8(cb, 0x01);
  cb_emit_u8(cb, 0xC0 | (src << 3) | dst);
}

int main() {
	uint8_t storage[1024];

	CodeBuffer cb;
	cb_init(&cb, storage, sizeof(storage));

	IRProgram prog = {0};

	parse_program(&prog, "return 60 + 5 + 2;");

	//ir_build_example(&prog);

	if (!ir_validate(&prog)) {
		printf("IR invalid\n");
		return 1;
	}

	for (size_t i = 0; i < prog.count; i++) {
            IRInstruction ins = prog.instructions[i];

	    printf("INS %d\n", ins.type);

            switch (ins.type) {
                case IR_CONST: {
                        int32_t dst = reg_code(ins.destination); 
			emit_mov_imm(&cb, dst, ins.a.as.value);
			break;
			       }
			
                case IR_ADD: {
              		int32_t dst = reg_code(ins.destination);
	      		int32_t a   = reg_code(ins.a.as.temp_id);
			int32_t b   = reg_code(ins.b.as.temp_id);

			if (dst == b) {
				emit_mov_reg(&cb, 10, b);
				b = 10;
			}

			if (dst != a) {
				emit_mov_reg(&cb, dst, a);
			}

			emit_add_reg(&cb, dst, b);
                        break;
			     }

                case IR_RETURN: {
			int32_t src = reg_code(ins.a.as.temp_id);

			emit_mov_reg(&cb, 7, src);

			cb_emit_u8(&cb, 0xB8);
			cb_emit_u32(&cb, 60);

			cb_emit_u8(&cb, 0x0F);
			cb_emit_u8(&cb, 0x05);
                        break;
				}
            }
        }

	elf_write("a.out", cb.data, cb.size);

	printf("aot-javac bootstrap\n");
	return 0;
}
