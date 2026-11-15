#include "runtime.h"

#include <cstdio>

#include <cstdlib>
#include <limits>

namespace {
    struct ArrayHeader {
        std::size_t length;
        std::size_t elementSize;
    };
}

extern "C"
void *jaot_array_allocate(std::size_t length, std::size_t elementSize) {
    if (elementSize != 1 &&
        elementSize != 2 &&
        elementSize != 4 &&
        elementSize != 8) {
        std::abort();
    }

    if (length > (std::numeric_limits<std::size_t>::max() - sizeof(ArrayHeader)) / elementSize) {
        std::abort();
    }

    const std::size_t bytes = sizeof(ArrayHeader) + length * elementSize;

    auto *header = static_cast<ArrayHeader *>(std::calloc(1, bytes));

    if (header == nullptr) {
        std::abort();
    }

    header->length = length;
    header->elementSize = elementSize;

    return header + 1;
}

extern "C"
void *jaot_array_element(void *array, std::int64_t index, std::size_t elementSize) {
    if (array == nullptr) {
        jaot_null_dereference();
    }

    auto *header = static_cast<ArrayHeader *>(array) - 1;

    if (header->elementSize != elementSize ||
        index < 0 ||
        static_cast<std::uint64_t>(index) >= header->length) {
        jaot_array_bounds_failure();
    }

    return static_cast<unsigned char *>(array) + static_cast<std::size_t>(index) * elementSize;
}

extern "C"
void jaot_array_bounds_failure() {
    std::abort();
}

extern "C"
void jaot_print_char(const unsigned char value) {
    std::printf("%c\n", value);
}

extern "C"
void jaot_print_byte(const signed char value) {
    std::printf("%hhd\n", value);
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
