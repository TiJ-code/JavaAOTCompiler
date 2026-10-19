# Stage 0

The IR now carries complete type and layout information instead of making the
backend reconstruct it. Locals, expressions, and fields use `TypeRef`; fields
record their offsets and sizes; and classes record their total allocation size.

Calls now carry resolved `MethodSignature` values, including the owning class,
return type, and parameter types. Lowering also performs stricter checks for
field access, assignments, returns, and call arguments. Code generation was
updated to consume this typed metadata directly.

The lowering test now checks typed fields, layout metadata, resolved calls, and
runtime-call signatures. The compiler and test target build successfully.
