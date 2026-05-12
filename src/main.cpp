#include <stdio.h>
#include "ast/ast.h"
#include "semantic/analyzer.h"
#include "semantic/symbol_table.h"
#include "parser/parser.h"
#include "ir/ir.h"
#include "ir/ir_builder.h"
#include "codegen/x86.h"

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

	SymbolTable table;
	symbol_table_init(&table);

	semantic_analyze(root, &table);
	ast_print(root, 0);

	IRFunction ir;
	ir_lower(root, &ir, &table);

	ir_print(&ir);

	FILE *out = fopen("out.s", "w");
	x86_generate(&ir, out);
	fclose(out);

	ir_free(&ir);

	printf("Code generation complete: out.s");

	symbol_table_free(&table);

	return 0;
}
