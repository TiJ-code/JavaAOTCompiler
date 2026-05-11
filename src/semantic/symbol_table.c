#include "semantic/symbol_table.h"
#include <string.h>
#include <stdlib.h>

static inline void symbol_table_resize(SymbolTable *table) {
	if (table->scope_count >= table->scope_capacity) {
		table->scope_capacity *= 2;
		table->scopes = realloc(
				table->scopes,
				sizeof(Scope) * table->scope_capacity
				);
	}
}

static inline void scope_resize(Scope *scope) {
	if (scope->count >= scope->capacity) {
		scope->capacity *= 2;
		scope->symbols   = realloc(
				scope->symbols,
				sizeof(Symbol) * scope->capacity
				);
	}
}

static void scope_add_symbol(Scope *scope, const char *name, Type *type, int32_t level) {
	scope_resize(scope);

	scope->symbols[scope->count].name        = strdup(name);
	scope->symbols[scope->count].type        = type;
	scope->symbols[scope->count].scope_level = level;

	scope->count++;
}

void symbol_table_init(SymbolTable *table) {
	table->scope_count    = 0;
	table->scope_capacity = 4;

	table->scopes = malloc(sizeof(Scope) * table->scope_capacity);
}

void symbol_table_free(SymbolTable *table) {
	while (table->scope_count > 0) {
		symbol_table_pop_scope(table);
	}

	free(table->scopes);
}

void symbol_table_push_scope(SymbolTable *table) {
	symbol_table_resize(table);

	Scope *scope = &table->scopes[table->scope_count++];

	scope->count = 0;
	scope->capacity = 4;

	scope->symbols = malloc(sizeof(Symbol) * scope->capacity);
}

void symbol_table_pop_scope(SymbolTable *table)  {
	if (table->scope_count == 0)
		return;

	Scope *scope = &table->scopes[table->scope_count - 1];

	for (size_t i = 0; i < scope->count; i++) {
		free(scope->symbols[i].name);
	}

	free(scope->symbols);

	table->scope_count--;
}

bool symbol_table_define(SymbolTable *table, const char *name, Type *type) {
	if (table->scope_count == 0)
		symbol_table_push_scope(table);

	Scope *scope = &table->scopes[table->scope_count - 1];

	for (size_t i = 0; i < scope->count; i++) {
		if (strcmp(scope->symbols[i].name, name) == 0)
			return false;
	}

	scope_add_symbol(scope, name, type, (int32_t)table->scope_count - 1);

	return true;
}

bool symbol_table_exists(SymbolTable *table, const char *name) {
	for (int32_t s = (int32_t)table->scope_count - 1; s >= 0; s--) {
		Scope *scope = &table->scopes[s];

		for (size_t i = 0; i < scope->count; i++) {
			if (strcmp(scope->symbols[i].name, name) == 0)
				return true;
		}
	}

	return false;
}

