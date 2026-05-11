#pragma once

#include "ast/ast.h"
#include "ir/ir.h"
#include "semantic/symbol_table.h"

#ifdef __cplusplus
extern "C" {
#endif

IRValue ir_lower_expr(ASTNode *node, IRFunction *f, SymbolTable *table);

void ir_lower(ASTNode *root, IRFunction *f, SymbolTable *table);

#ifdef __cplusplus
}
#endif
