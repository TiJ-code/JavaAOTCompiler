#include "semantic/type.h"

Type TYPE_LONG_OBJ = {
	.kind = TYPE_LONG,
	.size = 8
};

Type TYPE_INT_OBJ = {
	.kind = TYPE_INT,
	.size = 4
};

Type TYPE_SHORT_OBJ = {
	.kind = TYPE_INT,
	.size = 2
};

Type TYPE_BYTE_OBJ = {
	.kind = TYPE_INT,
	.size = 1
};

Type TYPE_VOID_OBJ = {
	.kind = TYPE_VOID,
	.size = 0
};

Type TYPE_UNKNOWN_OBJ = {
	.kind = TYPE_UNKNOWN,
	.size = 0
};
