#pragma once

#ifndef _ANALYZER_H
#define _ANALYZER_H

#include "ast/ast.h"
#include "semantic/symbol_table.h"

#ifdef __cplusplus
extern "C" {
#endif

	void semantic_analyze(ASTNode *root, SymbolTable *table);

#ifdef __cplusplus
}
#endif

#endif
