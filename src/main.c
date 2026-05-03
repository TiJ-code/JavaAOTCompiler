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

	for (int i = 0; i < prog.count; i++) {
            IRInstruction ins = prog.instructions[i];

            switch (ins.type) {

                case IR_CONST:
                    // mov rax, value
                    cb_emit_u8(&cb, 0x48);
                    cb_emit_u8(&cb, 0xC7);
                    cb_emit_u8(&cb, 0xC0);
                    cb_emit_u32(&cb, ins.value);
                    break;

                case IR_ADD:
                    // pop into rbx, add rax, rbx
                    cb_emit_u8(&cb, 0x48);
                    cb_emit_u8(&cb, 0x01);
                    cb_emit_u8(&cb, 0xD8);
                    break;

                case IR_RETURN:
                    // exit syscall rdi = rax
                    cb_emit_u8(&cb, 0x48);
                    cb_emit_u8(&cb, 0x89);
                    cb_emit_u8(&cb, 0xC7);

                    cb_emit_u8(&cb, 0xB8);
                    cb_emit_u32(&cb, 60);

                    cb_emit_u8(&cb, 0x0F);
                    cb_emit_u8(&cb, 0x05);
                    break;
            }
        }

	elf_write("a.out", cb.data, cb.size);

	printf("aot-javac bootstrap\n");
	return 0;
}
