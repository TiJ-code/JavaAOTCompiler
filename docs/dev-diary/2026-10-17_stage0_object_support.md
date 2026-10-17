# Stage 0

Today JAOT0 took its first steps from a language that can describe classes to a
compiler that can allocate and keep references to their objects.

The object work started by extending the frontend and IR to retain multiple
classes and their fields. The runtime gained a zero-initializing allocator, so
generated code can request storage for an object and rely on its fields starting
at their default values.

I then added syntax for `null` and `new ClassName()`, carried those expressions
through semantic analysis and IR lowering, and generated calls to
`jaot_allocate`. Null is represented as a zero reference, and equality on
references compares their addresses.

Class storage sizing now accounts for each field: four bytes for `int` and
`boolean`, and eight bytes for a reference. The backend can calculate an
allocation size from the class's fields, although it does not yet emit named
field offsets or field access instructions.

Reference locals were the next important step. A program can now declare a
class-typed local, initialize it with `new`, compare it to `null`, and assign
`null` back to it. The backend reserves eight bytes for a reference local and
uses 64-bit moves to store and load its address.

The first reference-local test exposed a parser gap: it accepted primitive
declarations but treated `Node node = ...` as an expression. Adding class-type
declaration parsing fixed that and gave the lowerer a chance to reject unknown
class names.

Finally, I started preserving the specific class name of each reference through
semantic analysis and into the IR. Until now, every object type collapsed into
the generic `Reference` type, which meant the compiler could not distinguish a
`Node` from an `Other`. The new semantic checks use that class name when
validating reference initializers, assignments, and equality comparisons.

The next step is field access. With class names available on reference locals
and field declarations already represented in the IR, the compiler can resolve
field names, calculate offsets, and generate loads and stores.
