#include "semantic/type.h"

Type TYPE_INT_OBJ = {
	.kind = TYPE_INT,
	.size = 4
};

Type TYPE_VOID_OBJ = {
	.kind = TYPE_VOID,
	.size = 0
};

Type TYPE_UNKNOWN_OBJ = {
	.kind = TYPE_UNKNOWN,
	.size = 0
};
