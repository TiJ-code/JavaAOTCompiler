# Stage 0

Today I started putting the actual compiler front end together.

Up until this point the project was mostly scaffolding structures. That was useful for getting the repository into a shape that I actually want to work in, but it was time to give the bootstrap compiler some real structure.

The first thing I added was the token layer.

`TokenKind` is intentionally pretty small right now. There are identifiers and integers, the handful of keywords the first language subset needs, some punctuation, and the basic arithmetic operators. Nothing fancy yet.
I would rather have a tiny language that is easy to understand than start implementing half of Java before the compiler can even compile itself.


I also added source positions to tokens. It is a small thing, but I want errors to be useful from the beginning, just like JAI. Having line and column information available right from the beginning should save some annoying refactoring later, that I had with my Registermaschine compiler implementation.

The parser got its first AST as well. It currently has expressions for integers, variables, binary operations and calls. Statements are similary limited to variable declarations, expressions and returns.

That is enough structure to start expressing small programs without making the AST overly complicated.

The parser interface follows the usual progression from expressions down to primaries:

expression --> term --> factor --> primary

It is deliberately simple for now. There is no attempt at supporting full Java expression grammar. Stage 0 is supposed to be a bootstrap compiler.

I also added the first `CodeGenerator` interface. There is not much behind it yet, but having the parser produce a `Program` that can eventually be handed to the generator gives the project clean boundaries.

source --> lexer --> tokens --> parser --> AST --> code generator

That pipline is also used in the Registermaschine and going to change as the compiler grows.

For now these are mostly interfaces and data structures. The next step is to implement them.





Pretty much all is based on my "assembly" compiler implementation in the [Registermaschine](https://github.com/TiJ-code/Registermaschine).
