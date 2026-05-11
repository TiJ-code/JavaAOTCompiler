#include "ir/ir.h"

#include <stdlib.h>
#include <string.h>

static void ensure(IRFunction *f) {
	if (f->count >= f->capacity) {
		f->capacity = f->capacity ? f->capacity * 2 : 8;
		f->instrs = realloc(f->instrs, f->capacity * sizeof(IRInstr));
	}
}

void ir_init(IRFunction *f) {
	f->instrs = NULL;
	f->count = 0;
	f->capacity = 0;
}

void ir_emit(IRFunction *f, IRInstr ins) {
	ensure(f);
	f->instrs[f->count++] = ins;
}

void ir_free(IRFunction *f) {
	free(f->instrs);
	f->instrs = NULL;
	f->count = f->capacity = 0;
}
