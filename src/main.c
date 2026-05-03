#include <stdio.h>
#include <stdint.h>
#include "elf_writer.h"

int main() {
	uint8_t code[] = {
		0xB8, 0x3C, 0x00, 0x00, 0x00,
		0xBF, 0x05, 0x00, 0x00, 0x00,
		0x0F, 0x05
	};

	elf_write("a.out", code, sizeof(code));

	printf("aot-javac bootstrap\n");
	return 0;
}
