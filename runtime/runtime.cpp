#include "runtime.h"

#include <cstdio>

#include <cstdlib>

extern "C"
void jaot_print_char(const unsigned char value) {
    std::printf("%c\n", value);
}

extern "C"
void jaot_print_short(const short value) {
    std::printf("%hd\n", value);
}

extern "C"
void jaot_print_int(const int value) {
    std::printf("%d\n", value);
}

extern "C"
void jaot_print_long(const long long value) {
    std::printf("%lld\n", value);
}

extern "C"
void jaot_print_bool(const int value) {
    std::printf(value ? "true\n" : "false\n");
}

extern "C"
void *jaot_allocate(const std::size_t size) {
    void *memory = std::calloc(1, size == 0 ? 1 : size);
    if (memory == nullptr) {
        std::abort();
    }
    return memory;
}

extern "C"
void jaot_null_dereference() {
    std::abort();
}
