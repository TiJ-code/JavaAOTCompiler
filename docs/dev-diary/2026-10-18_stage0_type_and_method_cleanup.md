# Stage 0

Today the JAOT0 compiler became more deliberate about the information it
carries through its stages. The language surface did not grow dramatically;
instead, the compiler stopped throwing away type and method information too
early.

The first cleanup was in semantic analysis. Expression checking now returns a
complete `TypeRef` rather than only a broad `Type` value. That means an
expression can retain the class name of a reference all the way through field
access, assignments, method arguments, and return statements. The old
separate reference-class inference path was removed.

The shared type utilities came next. `sameType`, `isAssignable`, and
`typeName` now define the common rules for exact type equality, reference
assignment, `null`, and diagnostic names. This removed several slightly
different versions of the same checks from semantic analysis and made the
intent of those checks easier to see.

Method signatures received the same treatment. A shared `MethodSignature`
structure now contains the method name, owning class, return `TypeRef`, and
parameter types. Semantic analysis, IR lowering, and code generation all use
that structure instead of maintaining separate return-type and class-name
pairs. The IR no longer stores method names and return types as unrelated
fields.

The language specification already described calls such as
`ClassName.method(...)`, so the parser and compiler were brought into line
with it. Qualified static calls now travel through the AST and IR, and the
semantic analyzer checks that the named class owns the method. The test suite
now includes a call from `Main` into a method declared by `Helper`.

The next refinement made method resolution genuinely class-scoped. Methods
are indexed by `ClassName.methodName`, unqualified calls resolve in the
current class, and identical method names are allowed in different classes.
The backend now emits collision-free symbols such as `jaot_Main_same` and
`jaot_Helper_same`, while preserving `jaot_main` for the runtime entry point.

Finally, semantic diagnostics were made more useful. Type errors now report
the expected and actual types for operands, arguments, initializers,
assignments, returns, and conditions. This turns errors such as a failed
reference assignment from a generic rejection into a message that explains
which types were involved.

