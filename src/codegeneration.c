#include "codegeneration.h"
#include <string.h>

void cb_init(CodeBuffer *cb, uint8_t *storage, size_t capacity) {
    cb->data     = storage;
    cb->size     = 0;
    cb->capacity = capacity;
}

void cb_reset(CodeBuffer *cb) {
    cb->size = 0;
}

size_t cb_size(CodeBuffer *cb) {
    return cb->size;
}

void cb_emit_u8(CodeBuffer *cb, uint8_t byte) {
    if (cb->size >= cb->capacity)
	   return;
    cb->data[cb->size++] = byte;
}

void cb_emit_u32(CodeBuffer *cb, uint32_t value) {
    cb_emit_u8(cb, (uint8_t)(value & 0xFF));
    cb_emit_u8(cb, (uint8_t)((value >> 8) & 0xFF));
    cb_emit_u8(cb, (uint8_t)((value >> 16) & 0xFF));
    cb_emit_u8(cb, (uint8_t)((value >> 24) & 0xFF));
}
