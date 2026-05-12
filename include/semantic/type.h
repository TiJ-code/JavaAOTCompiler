#pragma once

#include <stdint.h>

typedef enum {
	TYPE_UNKNOWN,
	TYPE_VOID,
	TYPE_BYTE,
	TYPE_SHORT,
	TYPE_INT,
	TYPE_LONG
} TypeKind;

typedef struct Type {
	TypeKind kind;
	uint32_t size;
} Type;

extern Type TYPE_LONG_OBJ;
extern Type TYPE_INT_OBJ;
extern Type TYPE_SHORT_OBJ;
extern Type TYPE_BYTE_OBJ;
extern Type TYPE_VOID_OBJ;
extern Type TYPE_UNKNOWN_OBJ;

static inline const char *type_name(Type *t) {
	switch (t->kind) {
		case TYPE_LONG:  return "long";
		case TYPE_INT:   return "int";
		case TYPE_SHORT: return "short";
		case TYPE_BYTE:  return "byte";
		case TYPE_VOID:  return "void";
		default:         return "unknown";
	}
}

