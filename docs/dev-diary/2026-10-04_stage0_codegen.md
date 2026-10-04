# Stage 0

The compiler compiles. And the output actually runs.

Over the last few days I had the lexer and the parser interfaces. Today all of it finally got connected, and a `.jaot` file can now go all the way to native executable that prints `42`.

The first thing was the parser implementation. It follows the structure I planned out earlier.

```text
expression -> term -> factor -> primary
```

`+` and `-` live in `parseExpression`, `*` and `/` live in `parseTerm`, which gives the usual precedence without any special handling. Unary minus is handled in `parseFactor` by rewriting `-x` into `0 - x`. This is not the most elegant solution, but it means the AST does not need a separate unary node yet.

Parser errors carry line and column, using the positions I attached to the tokens earlier. That already paid off today, since I made a few typos in test programs and the compiler told me exactly where.

After that came the code generator, which emits x86-64 assembly directly as text.

The expression strategy is deliberately naive. Evaluate the left side, push `%rax`, evaluate the right side, move it into `%eax`, pop the left side back into `%rax`, then apply the operation. Every expression leaves its result in `%eax`, so the generator never has to think about registers. It is slow, but it is also very hard to get wrong.

I got variables wrong at first, though. My first version just pushed the initializer onto the stack and called it a day, with no way to ever find it again. Variables reads threw "not implemented".

Now every `int` local gets a fixed slot below `%rbp`. A small pass collects all the locals before generating the body, reserves the stack space in the prologue and remembers the offset for each name. Reading a variable is a `movl` from that offset. Declaring the same variable twice is an error, and so is using an unknown one.

The runtime is tiny for now. It is a static library with exactly one function, `jaot_print_int`, which calls `printf`. The generator treats `printInt` as a built-in and emits a call to it.

I also wrote the CLI in `main.cpp`:

```bash
jaot0 input.jaot -S -o output.s
```

It only supports `-S` for now, since assembly is the only thing stage 0 can emit. Everything else is rejected with an error message.