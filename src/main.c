#include <stdio.h>
#include <stdint.h>
#include "elf_writer.h"
#include "codegeneration.h"

int main() {
	uint8_t storage[1024];

	CodeBuffer cb;
	cb_init(&cb, storage, sizeof(storage));

	cb_emit_u8(&cb, 0xB8);
	cb_emit_u32(&cb, 60);
	
	cb_emit_u8(&cb, 0xBF);
	cb_emit_u32(&cb, 5);

	cb_emit_u8(&cb, 0x0F);
	cb_emit_u8(&cb, 0x05);

	/*
	uint8_t code[] = {
		0xB8, 0x3C, 0x00, 0x00, 0x00,
		0xBF, 0x05, 0x00, 0x00, 0x00,
		0x0F, 0x05
	};*/

	elf_write("a.out", cb.data, cb.size);

	printf("aot-javac bootstrap\n");
	return 0;
}
