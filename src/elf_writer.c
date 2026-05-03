#include "elf_writer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PF_X 4
#define PF_R 1

typedef struct {
	unsigned char e_ident[16];
	uint16_t e_type;
	uint16_t e_machine;
	uint32_t e_version;
	uint64_t e_entry;
	uint64_t e_phoff;
	uint64_t e_shoff;
	uint32_t e_flags;
	uint16_t e_ehsize;
	uint16_t e_phentsize;
	uint16_t e_phnum;
	uint16_t e_shentsize;
	uint16_t e_shnum;
	uint16_t e_shstrndx;
} Elf64_Ehdr;

typedef struct {
	uint32_t p_type;
	uint32_t p_flags;
	uint64_t p_offset;
	uint64_t p_vaddr;
	uint64_t p_paddr;
	uint64_t p_filesz;
	uint64_t p_memsz;
	uint64_t p_align;
} Elf64_Phdr;

void elf_write(const char *filename, const uint8_t *code, size_t size) {
	FILE *f = fopen(filename, "wb");
	if (!f) {
		perror("fopen");
		return;
	}

	Elf64_Ehdr eh = {0};

	memcpy(eh.e_ident, "\x7F""ELF", 4);
	eh.e_ident[4] = 2; // 64bit
	eh.e_ident[5] = 1; // little endian
	eh.e_ident[6] = 1; // ELF version
	eh.e_ident[7] = 0; // ABI (System V)

	eh.e_version = 1;	
	eh.e_type    = 2;
	eh.e_machine = 0x3E;
	eh.e_phnum   = 1;

	eh.e_ehsize    = sizeof(Elf64_Ehdr);
	eh.e_phentsize = sizeof(Elf64_Phdr);
	eh.e_phnum     = 1;

	eh.e_phoff = sizeof(Elf64_Ehdr);

	uint64_t base_addr = 0x400000;
	uint64_t code_offset = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr);
	eh.e_entry = base_addr + code_offset;

	Elf64_Phdr ph = {0};

	ph.p_type   = 1;
	ph.p_flags  = PF_X | PF_R;
	ph.p_offset = 0;
	ph.p_vaddr  = 0x400000;
	ph.p_paddr  = 0x400000;
	ph.p_align = 0x1000;

	ph.p_filesz = sizeof(Elf64_Ehdr) + sizeof(Elf64_Phdr) + size;
	ph.p_memsz  = ph.p_filesz;

	fwrite(&eh, sizeof(eh), 1, f);
	fwrite(&ph, sizeof(ph), 1, f);
	fwrite(code, size, 1, f);

	fclose(f);

	char cmd[256];
	snprintf(cmd, sizeof(cmd), "chmod +x %s", filename);
	system(cmd);
}
