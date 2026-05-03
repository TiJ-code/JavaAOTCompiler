#include <stdio.h>
#include <stdint.h>

#include "elf_writer.h"
#include "codegeneration.h"
#include "ir.h"

int main() {
	uint8_t storage[1024];

	CodeBuffer cb;
	cb_init(&cb, storage, sizeof(storage));

	IRProgram prog;
	ir_build_example(&prog);

	int regs[8] = {0};

	for (size_t i = 0; i < prog.count; i++) {
            IRInstruction ins = prog.instructions[i];

            switch (ins.type) {
                case IR_CONST:
			regs[ins.destination] = ins.a.value;
			break;
			
                case IR_ADD:
			regs[ins.destination] =
				regs[ins.a.temp_id] + regs[ins.b.temp_id];
			break;

                case IR_RETURN:
			int32_t result = regs[ins.a.temp_id];

			// mov rax, result
	                cb_emit_u8(&cb, 0x48);
                        cb_emit_u8(&cb, 0xC7);
			cb_emit_u8(&cb, 0xC0);
			cb_emit_u32(&cb, result);

			// mov rdi, rax
			cb_emit_u8(&cb, 0x48);
			cb_emit_u8(&cb, 0x89);
			cb_emit_u8(&cb, 0xC7);

			// mov rax, 60
			cb_emit_u8(&cb, 0xB8);
                        cb_emit_u32(&cb, 60);

			// syscall
                        cb_emit_u8(&cb, 0x0F);
                        cb_emit_u8(&cb, 0x05);
                        break;
            }
        }

	elf_write("a.out", cb.data, cb.size);

	printf("aot-javac bootstrap\n");
	return 0;
}
