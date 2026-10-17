#include "runtime.h"

#include <cstdio>

#include <cstdlib>

extern "C"
void jaot_print_int(int value) {
    std::printf("%d\n", value);
}

extern "C"
void jaot_print_bool(int value) {
    std::printf(value ? "true\n" : "false\n");
}

extern "C"
void *jaot_allocate(std::size_t size) {
    void *memory = std::calloc(1, size == 0 ? 1 : size);
    if (memory == nullptr) {
        std::abort();
    }
    return memory;
}
