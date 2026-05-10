#pragma once

#ifndef _PARSER_H
#define _PARSER_H

#include "ast/ast.h"

ASTNode *parse_java_file(const char *path);

#endif
