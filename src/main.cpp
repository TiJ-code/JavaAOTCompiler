#include <stdio.h>
#include "ast/ast.h"
#include "parser/parser.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		printf("Usage: compiler <file.java>\n");
		return 1;
	}

	ASTNode *root = parse_java_file(argv[1]);

	if (!root) {
		printf("Parse error\n");
		return 1;
	}

	printf("Parsed successfully\n");

	return 0;
}
