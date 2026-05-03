#include "parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

static inline char peek(Lexer *l) {
	return l->src[l->pos];
}

static inline char advance(Lexer *l) {
	return l->src[l->pos++];
}

static void skip_ws(Lexer *l) {
	while (isspace(peek(l))) advance(l);
}

void lexer_init(Lexer *l, const char *src) {
	l->src = src;
	l->pos = 0;
}

Token lexer_next(Lexer *l) {
	skip_ws(l);

	char c = peek(l);

	if (c == '\0') return (Token){ TOK_EOF, 0 };

	if (isdigit(c)) {
		int32_t value = 0;
		while (isdigit(peek(l))) {
			value = value * 10 + (advance(l) - '0');
		}
		return (Token) { TOK_NUMBER, value };
	}

	if (c == '+') {
		advance(l);
		return (Token) { TOK_PLUS, 0 };
	}
	if (c == '-') {
		advance(l);
		return (Token) { TOK_MINUS, 0 };
	}
	if (c == '*') {
		advance(l);
		return (Token) { TOK_MULTIPLY, 0 };
	}
	if (c == '/') {
		advance(l);
		return (Token) { TOK_DIVIDE, 0 };
	}

	if (c == ';') {
		advance(l);
		return (Token) { TOK_SEMI, 0 }; 
	}

	if (strncmp(&l->src[l->pos], "return", 6) == 0) {
		l->pos += 6;
		return (Token) { TOK_RETURN, 0 };
	}

	printf("Lexer error at %c\n", c);
	exit(1);
}

static int32_t temp_id = 1;

static int32_t emit_const(IRProgram *p, int32_t value) {
	int t = temp_id++;

	p->instructions[p->count++] = (IRInstruction){
		.type = IR_CONST,
		.a = ir_const(value),
		.destination = t
	};

	return t;
}

static int32_t emit_arithmetic(IRType type, IRProgram *p, int32_t a, int32_t b) {
	int t = temp_id++;

	p->instructions[p->count++] = (IRInstruction){
		.type = type,
		.a = ir_temp(a),
		.b = ir_temp(b),
		.destination = t
	};

	return t;
}

static int32_t emit_add(IRProgram *p, int32_t a, int32_t b) {
	return emit_arithmetic(IR_ADD, p, a, b);
}

static int32_t emit_sub(IRProgram *p, int32_t a, int32_t b) {
	return emit_arithmetic(IR_SUB, p, a, b);
}

static int32_t emit_mul(IRProgram *p, int32_t a, int32_t b) {
	return emit_arithmetic(IR_MUL, p, a, b);
}

static int32_t emit_div(IRProgram *p, int32_t a, int32_t b) {
	return emit_arithmetic(IR_DIV, p, a, b);
}

static void next(Parser *p) {
	p->current = lexer_next(&p->lex);
}

static int32_t parse_factor(Parser *p) {
	if (p->current.type == TOK_NUMBER) {
		int32_t value = p->current.value;
		next(p);
		return emit_const(p->out, value);
	}

	printf("Parser error: expected number\n");
	exit(1);
}

static int32_t parse_term(Parser *p) {
	int32_t left = parse_factor(p);

	while (p->current.type == TOK_MULTIPLY || p->current.type == TOK_DIVIDE) {
		TokenType op = p->current.type;
		next(p);

		int32_t right = parse_factor(p);

		if (op == TOK_MULTIPLY)
			left = emit_mul(p->out, left, right);
		else
			left = emit_div(p->out, left, right);
	}

	return left;
}

static int32_t parse_expr(Parser *p) {
	int left = parse_term(p);

	while (p->current.type == TOK_PLUS || p->current.type == TOK_MINUS) {
		TokenType op = p->current.type;
		next(p);

		int32_t right = parse_term(p);

		if (op == TOK_PLUS)
			left = emit_add(p->out, left, right);
		else
			left = emit_sub(p->out, left, right);
	}

	return left;
}

void parse_program(IRProgram *out, const char *src) {
	Parser p;
	p.out = out;

	lexer_init(&p.lex, src);
	next(&p);

	if(p.current.type != TOK_RETURN) {
		printf("Parser error: expected 'return'\n");
		exit(1);
	}

	next(&p);

	int32_t result = parse_expr(&p);

	if (p.current.type != TOK_SEMI) {
		printf("Parser error: expected ';'\n");
		exit(1);
	}

	out->instructions[out->count++] = (IRInstruction){
		.type = IR_RETURN,
		.a = ir_temp(result),
		.destination = 0
	};
}
