#pragma once
#include <cstddef>
#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

void *jaot_allocate(std::size_t size);
void jaot_null_dereference();

void *jaot_array_allocate(std::size_t length, std::size_t elementSize);
void *jaot_array_element(void *array, std::int64_t index, std::size_t elementSize);
void jaot_array_bounds_failure();

void jaot_print_char(unsigned char value);
void jaot_print_byte(signed char value);
void jaot_print_short(short value);
void jaot_print_int(int value);
void jaot_print_long(long long value);
void jaot_print_bool(int value);

#ifdef __cplusplus
}
#endif