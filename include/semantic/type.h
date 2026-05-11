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

#endif
