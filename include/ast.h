#ifndef AST_H
#define AST_H

typedef enum {
	AST_CONST,
	AST_ADD,
	AST_SUB,
	AST_MUL,
	AST_DIV,
	AST_RETURN
} ASTType;

typedef struct ASTNode {
	ASTType type;

	int32_t value;
	struct ASTNode *left;
	struct ASTNode *right;
} ASTNode;

#endif
