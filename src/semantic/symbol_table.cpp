#include "semantic/symbol_table.h"
#include <stdint.h>

void SymbolTable::define( const std::string &name, int32_t value )
{
    globals[name] = value;
}

bool SymbolTable::get( const std::string &name, int32_t &out )
{
    auto it = globals.find( name );
    if (it == globals.end())
        return false;

    out = it->second;
    return true;
}
