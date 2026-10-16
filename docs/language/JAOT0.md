# JAOT0 Language Specification

## 1. Purpose

**JAOT0** is the initial source language for the Java Ahead Of Time Compiler project.

It is intentionally much smaller than Java. JAOT0 exists to bootstrap the compiler and provide a simple language that can eventually be used to implement the next compiler stage.

The goal is:
```text
JAOT0 source
   -> JAOT0 compiler
   -> native x86-64 executable
```

JAOT0 uses Java-like syntax, but it does not attempt to implement the Java language or JVM semantics.

## 2. Example Program

A complete JAOT0 program can look like this:

```java
class Main {
    static void main() {
        int answer = 20 + 22;
        printInt(answer);
    }
}
```

The expected result is:
```text
42
```

A slightly larger example:

```java
class Main {
    static int add(int a, int b) {
        return a + b;
    }
    
    static void main() {
        int result = add(20, 22);
        printInt(result);
    }
}
```

## 3. Program Structure

A JAOT0 source file contains exactly one class.

```text
program
    ::= class-declaration
```

A class declaration has the form:

```java
class ClassName {
}
```

For stage 0, the class name does not represent a runtime object type. It primarily provides the outer structure of the source program.

## 4. Classes

The basic syntax is:

```java
class Identifier {
    method*
}
```

JAOT0 does not currently support:

- fields
- constructors
- inheritance
- interfaces
- nested classes
- generics
- annotations
- access modifiers
- instance methods
- `new`

These belong to later language stages.

## 5. Methods

Methods are declared using:

```java
static ReturnType methodName(ParameterList) {
    statements
}
```

## 6. Entry Point

Every executable JAOT0 program must provide:
```java
static void main() {
}
```

## 7. Types

JAOT0 initially supports a very small type system.

## 7.1 `int`

A signed 32-bit integer.

```java
int x = 42;
```

Integer literals:

```java
0
1
42
-10
123456
```

Integer values use signed 32-bit semantics. The valid literal range is
`-2147483648` through `2147483647`. Addition, subtraction, and multiplication
wrap modulo 2^32 when their mathematical result is outside that range.

## 7.2 `void`

`void` means that a method does not return a value.

```java
static void hello() {
    printInt(42);
}
```

A `void` method cannot be used as an expression.

## 7.3 `boolean`

`boolean` is a value type whose only values are `true` and `false`. It can be
used as a method parameter or return type, and as a local variable type:

```java
boolean enabled = true;

static boolean isPositive(int value) {
    return value > 0;
}
```

Boolean and integer values are distinct types. JAOT0 does not implicitly
convert between them. `true` and `false` are boolean literals.

## 7.4 Future types

The following are intentionally reserved for later stages:

```text
long
char
reference types
arrays
String
```

## 8. Variables

Local variables of type `int` or `boolean` can be declared inside methods.
They may be declared with or without an initializer:

```java
int answer;
int initialized = 42;
boolean enabled = true;
```

A variable declared without an initializer must be assigned before it is
read. Reading it earlier is a semantic error. Method parameters are initialized
when the method is called. Each block introduces a scope. A local variable is
visible from its declaration to the end of its containing block, including
nested blocks; a nested block may declare a variable with the same name as one
in an enclosing block.

## 9. Assignment

Local variables and method parameters are mutable. An assignment updates a
previously declared variable with a value of the same type; it does not declare
a new one. Assignment is a statement, not an expression:

```java
int count = 0;
count = count + 1;
```

An assignment is a statement, not an expression. Its target must name a
parameter or local variable that has already been declared in the method.
Assignment does not declare a new variable, and fields and global variables
are not supported.

```text
variable-declaration:
    value-type Identifier ("=" expression)? ;

assignment-statement:
    Identifier = expression ;
```

## 10. Expressions

JAOT0 supports integer and boolean expressions.

```java
1 + 2
20 + 22
10 * 5
100 - 25
value >= 0
enabled == false
```

Expressions can contain:

- integer literals
- boolean literals
- variables
- function calls
- arithmetic and comparison operators
- parentheses

## 11. Operators

The initial operators are:

| Operator | Meaning          |
|----------|------------------|
| `+`      | addition         |
| `-`      | subtraction      |
| `*`      | multiplication   |
| `/`      | integer division |

Integer division truncates toward zero. Dividing by zero terminates the
program abnormally. The otherwise-overflowing expression
`-2147483648 / -1` evaluates to `-2147483648`.

```java
int a = 10 + 5;
int b = 10 - 5;
int c = 10 * 5;
int d = 10 / 5;
```

Operator precedence:

```text
* /
+ -
< <= > >=
== !=
```

Operators at the same precedence level associate from left to right. Arithmetic
operators require `int` operands and produce an `int`. Relational operators
require `int` operands and produce a `boolean`. Equality operators require two
operands of the same type (`int` or `boolean`) and produce a `boolean`.
Relational comparisons use signed integer values. Logical operators such as
`&&`, `||`, and `!` are not part of this stage.

An expression's operands are evaluated from left to right. In particular,
comparisons do not short-circuit.

## 12. Function Calls

Methods can be called using:

```text
methodName(argument0, argument1); 
```

```java
static int add(int a, int b) {
    return a + b;
}

static void main() {
    int result = add(20, 22);
    printInt(result);
}
```

Arguments are expressions:

```java
printInt(20 + 22);
int x = add(10 * 2, 20 + 2);
```

## 13. Stack Frames and Calling Convention

The initial native backend targets x86-64 System V and uses a frame pointer for
each method. Each parameter and local variable gets a four-byte stack slot.
`int` values use signed 32-bit representation; `boolean` values use `0` for
`false` and `1` for `true`. Parameters occupy slots just like locals; the
method prologue copies incoming arguments into those slots so that all variable
references use the same frame-relative addressing.

The frame layout grows down from `%rbp`: the first parameter is stored at
`-4(%rbp)`, the next parameter at `-8(%rbp)`, followed by local variables.
Space for every slot is reserved once in the method prologue. The total frame
allocation is rounded up to a multiple of 16 bytes to preserve the required
stack alignment for calls.

The prologue saves the caller's frame pointer, establishes the new `%rbp`, and
reserves the aligned frame allocation. Methods return `int` and `boolean`
results in `%eax`; boolean results use the same `0`/`1` representation. The
epilogue uses `leave` and `ret` to restore the caller's frame and return address.

`int` and `boolean` arguments use the x86-64 System V integer argument
registers in order: `%rdi`, `%rsi`, `%rdx`, `%rcx`, `%r8`, and `%r9`. The callee
copies the corresponding 32-bit register values into its parameter slots.
JAOT0 currently supports at most six parameters per method; stack-passed
arguments are not implemented.

Call arguments are evaluated from left to right and temporarily pushed so
evaluating a later argument cannot overwrite an earlier result. The values are
then moved into the argument registers and the call is emitted. Temporary
expression pushes are tracked by the code generator, which adds eight bytes of
padding when needed so the stack is 16-byte aligned immediately before a call.
The temporary pushes and any call padding do not form part of the method's
fixed local-variable frame.

## 14. Built-in Runtime Functions

JAOT0 provides a very small runtime interface.

Initially:

```java
printInt(int value)
```

prints an integer followed by a newline.

```java
printInt(42);
```

outputs

```text
42
```

`printInt` is not a normal JAOT0 method implemented by the program. It is supplied by the native runtime.

## 15. Return Statements

A non-`void` method returns a value of its declared type:

```java
return expression;
```

A `void` method may use:

```java
return;
```

Every `int` and `boolean` method must return a value of its declared type on
every reachable path. A `void` method may either return with `return;` or reach
the end of its body. A return expression must have exactly the declared return
type; `void` methods cannot return a value.

## 16. Statements

JAOT0 statements include:

```text
variable declaration
assignment
expression statement
if statement
return statement
```

```java
int x = 42;
printInt(x);
if (x > 0) {
    printInt(x);
} else {
    printInt(0);
}
```

An `if` statement evaluates its parenthesized condition once. The condition
must have type `boolean`; the `then` block executes when it is `true`, and the
optional `else` block executes when it is `false`. If there is no `else` and
the condition is false, execution continues after the `if`. Blocks may contain
nested `if` statements.

Every variable read must be definitely initialized on every path reaching the
read. After an `if`/`else`, a variable is definitely initialized only if it is
initialized at the end of every branch that can continue. For an `if` without
an `else`, assignments made only in the `then` block do not make a variable
definitely initialized after the statement. Return analysis likewise accounts
for both branches; both branches of an `if`/`else` must return for that
statement to guarantee a return. Flow analysis treats both branches as possible,
even when the condition is a literal.

## 17. Comments

JAOT0 should support normal Java-style comments.

Single-line:

```java
int x = 42;
printInt(x);
return x;
```

Multi-line:

```java
/*
 * This is a
 * multi-line comment.
 */
int x = 42;
```

Comments have no semantic meaning.

## 18. Whitespace

Whitespace separates tokens but otherwise has no semantic meaning;

```java
int x = 42;

int
x
=
42
;
```

Whitespace includes:

- space
- tab
- newline
- carriage return

## 19. Identifiers

Identifiers are used for:

- class names
- method names
- parameter names
- local variable names

Identifiers use letters, digits, and underscores. An identifier may start
with a letter or underscore; digits and underscores are also allowed after
the first character.

```text
identifier
    ::= (letter | "_") (letter | digit | "_")* ;
```

A digit cannot be the first character.

## 20. Keywords

Reserved keywords:

```java
class
static 
void
int
boolean
true
false
return
if
else
```

They cannot be used as identifiers.

## 21. Punctuation

JAOT0 uses:

```java
{ }
( )
;
,
=
+ - * /
== != < <= > >=
```

```java
class Main {
    static int add(int a, int b) {
        return a + b;
    }
}
```

## 22. Complete Minimal Grammar

The JAOT0 grammar can be described as:

```text
program
    ::= class-declaration EOF ;
    
class-declaration
    ::= "class" identifier
        "{"
        method-declaration*
        "}" ;

method-declaration
    ::= "static"
        type
        identifier
        "(" parameter-list? ")"
        block ;
        
parameter-list
    ::= parameter ("," parameter)* ;
    
parameter
    ::= value-type identifier ;

type
    ::= "int"
      | "boolean"
      | "void";

value-type
    ::= "int"
      | "boolean";

block
    ::= "{"
        statement*
        "}";

statement
    ::= block
      | variable-declaration
      | assignment-statement
      | expression-statement
      | if-statement
      | return-statement;

variable-declaration
    ::= value-type identifier ("=" expression)? ";" ;

assignment-statement
    ::= identifier "=" expression ";" ;

if-statement
    ::= "if" "(" expression ")" block
        ("else" block)? ;

expression-statement
    ::= expression ";" ;

return-statement
    ::= "return" expression? ";" ;

expression
    ::= equality ;

equality
    ::= comparison
        (("==" | "!=") comparison)* ;

comparison
    ::= addition
        (("<" | "<=" | ">" | ">=") addition)* ;

addition
    ::= multiplication
        (("+" | "-") multiplication)* ;

multiplication
    ::= unary
        (("*" | "/") unary)* ;

unary
    ::= "-" unary
      | primary ;

primary
    ::= integer
      | "true"
      | "false"
      | identifier
      | call
      | "(" expression ")" ;
      
call
    ::= identifier
        "(" argument-list? ")" ;
        
argument-list
    ::= expression ("," expression)* ;
    
integer
    ::= digit+ ;
    
identifier
    ::= (letter | "_") (letter | digit | "_")* ;
```

This grammar is intentionally small. The grammar describes syntax; semantic
analysis enforces the operand, condition, argument, assignment, and return
types described above. `void` is valid only as a method return type.
