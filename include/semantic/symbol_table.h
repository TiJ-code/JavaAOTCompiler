#pragma once

#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H


#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct Symbol {
	char *name;
	int32_t scope_level;
} Symbol;

typedef struct Scope {
	Symbol *symbols;
	size_t count;
	size_t capacity;
} Scope;

typedef struct SymbolTable {
	Scope *scopes;
	size_t scope_count;
	size_t scope_capacity;
} SymbolTable;

void symbol_table_init(SymbolTable *table);
void symbol_table_free(SymbolTable *table);

void symbol_table_push_scope(SymbolTable *table);
void symbol_table_pop_scope(SymbolTable *table);

bool symbol_table_define(SymbolTable *table, const char *name);
bool symbol_table_exists(SymbolTable *table, const char *name);

#ifdef __cplusplus
}
#endif

#endif
