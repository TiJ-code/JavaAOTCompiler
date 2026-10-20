# Stage 0

The code generator now has a small machine layer.

`Machine` owns the x86 details that were previously spread throughout
`codegen.cpp`: function prologues and epilogues, constants, frame loads and
stores, temporary pushes and pops, stack-depth tracking, call alignment,
argument registers, branches, integer operations, comparisons, and division
labels. The build now includes the new machine header and implementation.

Expressions return a `ValueLocation` instead of silently promising that
everything is in `%eax`. The implementation still uses the deliberately
simple accumulator strategy: integers are in `%eax`, references are in
`%rax`, and temporary values use the stack. This is not register allocation
yet; it is an explicit description of the current location invariant.

`codegen.cpp` still owns IR traversal and evaluation order, while `Machine`
emits the instructions and maintains the temporary-stack invariant. A small
amount of raw assembly remains for parameter initialization and field-memory
operands, but it now goes through the machine boundary.

The generated programs behave the same as before.
