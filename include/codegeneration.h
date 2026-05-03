#ifndef CODEGENERATION_H
#define CODEGENERATION_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
	uint8_t *data;
	size_t size;
	size_t capacity;
} CodeBuffer;

void cb_init(CodeBuffer *cb, uint8_t *storage, size_t capacity);
void cb_reset(CodeBuffer *cb);

void cb_emit_u8(CodeBuffer *cb, uint8_t byte);
void cb_emit_u32(CodeBuffer *cb, uint32_t value);

size_t cb_size(CodeBuffer *cb);

#endif
