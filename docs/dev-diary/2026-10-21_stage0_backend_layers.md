# Stage 0

The backend now separates runtime operations and expression lowering from the
main code-generation traversal.

Runtime-specific behavior was moved behind `RuntimeEmitter`. Allocation,
integer and boolean printing, and null-dereference failure are now expressed
as semantic runtime operations instead of being assembled directly in
`codegen.cpp`. `RuntimeEmitter` delegates the actual calling convention and
instruction emission to `Machine`, preserving the boundary between what a
runtime operation means and how x86-64 instructions implement it.

Expression lowering was then extracted into `ExpressionLowerer`. It now owns
the lowering of literals, locals, object creation, field access, unary and
binary operators, short-circuit logical operators, division, printing, and
method calls. It continues to use the explicit `ValueLocation` abstraction
and the existing stack-based machine implementation, so this refactoring does
not introduce register allocation or change the current value-location
invariants.

`FunctionGenerator` is consequently focused on function, block, statement,
terminator, and field-assignment traversal. The build was updated to include
the new expression-codegen implementation. A label naming collision between
expression field access and field assignment was also removed while preserving
the generated control flow.
