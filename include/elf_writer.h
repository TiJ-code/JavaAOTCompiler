#ifndef ELF_WRITER_H
#define ELF_WRITER_H

#include <stddef.h>
#include <stdint.h>

void elf_write(const char *filename, const uint8_t *code, size_t size);

#endif
