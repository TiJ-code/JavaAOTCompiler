#pragma once

#ifndef _SYMBOL_TABLE_H
#define _SYMBOL_TABLE_H

#include <unordered_map>
#include <string>

class SymbolTable {
public:
	std::unordered_map<std::string, int32_t> globals;

	void define(const std::string &name, int32_t value);
	bool get(const std::string &name, int32_t &out);
};

#endif
