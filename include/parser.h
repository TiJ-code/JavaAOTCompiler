#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>
#include <stdint.h>
#include "ir.h"

typedef enum {
	TOK_NUMBER,
	TOK_PLUS,
	TOK_MINUS,
	TOK_MULTIPLY,
	TOK_DIVIDE,
	TOK_RETURN,
	TOK_SEMI,
	TOK_EOF
} TokenType;

typedef struct {
	TokenType type;
	int32_t value;
} Token;

typedef struct {
	const char *src;
	size_t pos;
	Token current;
} Lexer;

typedef struct {
	Lexer lex;
	Token current;
	IRProgram *out;
} Parser;

void lexer_init(Lexer *l, const char *src);
Token lexer_next(Lexer *l);

void parse_program(IRProgram *out, const char *src);

#endif
