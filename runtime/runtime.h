#pragma once
#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif

void *jaot_allocate(std::size_t size);

void jaot_print_int(int value);
void jaot_print_bool(int value);

#ifdef __cplusplus
}
#endif