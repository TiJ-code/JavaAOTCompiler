#include "jaot/ir.h"

int main() {
    const JAOT::IR::Program program;
    return program.methods.empty() ? 0 : 1;
}