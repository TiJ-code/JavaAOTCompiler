#include "runtime.h"

#include <cstdio>

extern "C"
void jaot_print_int(int value) {
    std::printf("%d\n", value);
}
