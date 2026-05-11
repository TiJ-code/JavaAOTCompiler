#pragma once

#include "ir/ir.h"
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

void x86_generate(IRFunction *f, FILE *out);

#ifdef __cplusplus
}
#endif
