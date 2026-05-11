#pragma once

#ifndef _TYPE_H
#define _TYPE_H

#include <stdint.h>

typedef enum {
	TYPE_UNKNOWN,
	TYPE_VOID,
	TYPE_INT
} TypeKind;

typedef struct Type {
	TypeKind kind;
	uint32_t size;
} Type;

extern Type TYPE_INT_OBJ;
extern Type TYPE_VOID_OBJ;
extern Type TYPE_UNKNOWN_OBJ;

static inline const char *type_name(Type *t) {
	switch (t->kind) {
		case TYPE_INT:  return "int";
		case TYPE_VOID: return "void";
		default:        return "unknown";
	}
}

#endif
