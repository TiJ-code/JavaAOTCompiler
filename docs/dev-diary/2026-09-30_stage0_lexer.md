# Stage 0

The lexer is actually alive now.

The previous commits had the lexer interface and token definitions. This time I implemented the actual source-to-token pass.

The lexer currently understands the small subset of Java that stage 0 needs. That means identifiers, integer literals, the first few keywords, punctuation, assignment, and the basic arithmetic operators.

Keywords are handled through a small lookup table. Everything that looks like an identifier is scanned first and then checked against that table. This keeps the actual scanning logic pretty simple and means adding another keyword later does not require another branch in the lexer.

I also kept source positions attached to every token.

The lexer tracks both line and column while consuming characters, including resetting the column when a newline is encountered. This is one of those things that is very easy to leave until later and then annoying to retrofit once the parser and diagnostics already depend on the token structure.

Comments are supported as well, although only the `//` kind for now. The lexer skips whitespace and comments before producing the next token.

There is also explicit handling for unexpected characters. Instead of silently doing something weird with input that does not belong to the language, the lexer throws an error containing the current source position.

The overall pipeline now has its first real executable step:

source --> lexer --> tokens

The parser is still next in line, but at least there is now something real for it to consume.

I am deliberately keeping this implementation boring. There is no complicated token buffering, no attempt at supporting every Java literal, and no giant collection of language rules yet.

Stage 0 should grow one small piece at a time.

One thing I noticed while writing this is how closely this is already following the structure of the Registermaschine compiler. That project has been a pretty useful reference for how I want the compiler stages to be separated here as well.

Next up is getting the parser implementation working against these tokens.

