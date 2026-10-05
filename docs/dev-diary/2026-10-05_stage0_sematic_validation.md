# Stage 0

The compiler now says no. And it knows exactly when to.

Yesterday a `.jaot` file could go all the way to a native executable. Today was about making that path trustworthy. Real functions, a semantic pass that rejects broken programs before codegen ever sees them, and a proper definition of what integer actually do.

I started with division, since `/` was the last operator without codegen. It is `cltd` followed by `idivl %ecx`, and that part took five minutes. The more useful change was in the test setup: the test runner can now compare the program output against an `.expected` file instead of only checking that the program runs. The first test is a handful of divisions with mixed signs, to pin down that integer division trunactes towards zero.

Next came real functions. Until now everything lived in `main`, but a method can now take paramters and return values.

The calling convention is plain x86-64 System V. Arguments go through `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8` and `%r9`, which means six params at most for now. Stack-passed arguments are not implemented. The callee copies each incoming register into a parameter slot right in the prologue, so parameters and locals addressed in exactly the same way.

The part that needed care was the stack. Because the expression strategy pushes temporaries ass the time, the stack is not necessarily 16-byte aligned when a call happens.
The generator now tracks how many bytes it has pushed and inserts 8 bytes of padding right before a call when needed.
The frame itself is rounded up to a multiple of 16. The test is `add(20, add(10, 12))`, which prints `42` and exercises exactly the nested case where this goes wrong.

While I was in there I noticed the parser had no idea what a method returns, so I refactored it. There is now a `Type` enum, a `Parameter` struct, and a return type on every `Method`.

Every AST node now also carries a `SourceLocation`. Binary expressions point at their operator, calls and variables at their identifier, and the unary minus rewrite points at the minus.
This is what makes the next part useful.

The next part is the semantic analyzer, a separate pass between parser and code generator.
It checks:
- unknown variables and unknown functions
- argument count and argument types
- duplicate parameters, variables and functions
- that `printInt` cannot be redefined
- that `void` methods do not return a value

Errors look the parser errors with line and column:

```text
semantic error at 3:16: unknown variable: missing
```

The code generator still has its own checks, but they are now only a safety net.
I also added a second test runner that expects compilation to fail and looks for a specific message in stderr.

Here are some smaller additional rules I implemented:

- multi-line comments. `/* ... */` is skipped in the lexer. An unterminated comment is an error that points at where the comment started.
- entry point. A program must contain `static void main()`.
- integer bounds. Literals must fit into 32 bits, and I had to be careful with `-2147483648`. The number `2147483648` alone is out of range, but with a minus in front is valid. 
   So the parser now folds a minus directly followed by an integer into one literal and checks the range with the right limit.
   Everything else still goes through `0 - x`.
- return paths. An `int` method has to return a value. My first check was "does a return statement exist anywhere", which I changed to "can execution fall of the end".
   Without conditionals or loops both mean the same thing today, but the second one is the check that will keep working when `if` shows up.

The last piece was making division safe at runtime, because `idivl` has two nasty cases.
Dividing by zero raises a hardware exception, and `INT_MIN / -1` overflows and does the same thing.
The generator now emits a check before every division. A zero divisor jumps to a `ud2`, so the program dies deliberately.
`-2147483648 / -1` is defined to give `-2147483648`. Each division gets its own set of labels, built from the method name and a counter.

That also forced to define the other overflow cases, so the spec now says that `+`, `-` and `*` wrap module 2^32.
The division tests checks all that. For the division by zero case I added an `EXPECT_RUNTIME_FAILURE` flag to the test runner, which turns a crash into the expected result.

The language spec got updated along the way.
A new section on stack frames and the calling convention, the integer and division rules, the return rule,
the identifier syntax, and a grammar fix so that parameters are `"int" identifer`.

The next goal is not to keep the compiler in C++ forever: C++ is the trusted
bootstrap implementation, and JAOT0 should grow only until it can compile a
JAOT1 compiler written in JAOT0. Before adding multiple machine targets, the
compiler should gain a target-independent typed IR and a backend boundary.
That will let us add distinct x86-64/System V and ARM64/AAPCS64 code generators
without tying the language frontend to one architecture or ABI.