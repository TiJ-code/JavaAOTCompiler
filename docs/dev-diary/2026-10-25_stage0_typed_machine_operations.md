# Stage 0

The backend now uses typed machine operations instead of exposing raw
instruction text to expression and statement lowering.

`Machine` can now load and store typed values through memory addresses, save
values in explicit scratch-register locations, initialize frame slots from
calling-convention argument registers, and normalize boolean results. The
additional register locations make the current stack-based implementation
explicit without introducing register allocation.

Field loads and stores in expression and statement lowering now go through
these machine operations. Parameter initialization and boolean normalization
were moved behind the same boundary, and the remaining `emitRaw()` calls were
removed from code generation. Comparisons now use the supplied
`ValueLocation`, so reference operands preserved in `%rdx` are handled
correctly instead of being assumed to be in `%rcx`.
