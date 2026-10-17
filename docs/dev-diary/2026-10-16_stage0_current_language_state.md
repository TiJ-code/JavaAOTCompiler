# Stage 0

The compiler is now at the point where the language surface is small but real. The tests are no longer only checking that the frontend can parse a few happy paths; they are pinning down the current language state and making sure the behavior stays consistent as the backend grows.

Today's work was focused on that. I added coverage for boolean assignment and for the comparison operators that are already part of the language: `==`, `!=`, `<`, `<=`, `>`, and `>=`.

The new tests are deliberately simple and direct. They verify that:

- `boolean` values can be assigned and reassigned
- equality checks work for both `int` and `boolean`
- relational checks follow the expected signed-integer semantics
- `if` statements still branch correctly on the resulting boolean values

The first pass exposed a real problem in the lexer: the token for `<=` was being emitted as just `<`.
That meant the generated code compared with `<` instead of `<=`, which is exactly the kind of bug that slips past a happy-path test and only shows up when you start exercising the full operator set.

The fix was minimal but important: `<=` has to become its own token, with the correct text and kind, and then the backend uses the matching `setle` instruction. Once that was corrected, the language-state tests passed cleanly.

This was useful in a very specific way. It confirmed that the current stage is not just syntactically valid code; it is a coherent mini-language with a stable set of behaviors. The compiler now has stronger evidence for:

- integer and boolean types
- assignment and variable initialization rules
- comparison operators and boolean branching
- the fact that the frontend and the backend agree on the current language contract

The next step is no longer to add more syntax in a random way. The work should stay focused on the edges of the current language: more semantic checks, more control-flow cases, and any missing operators or execution paths that are still not pinned down by tests.
