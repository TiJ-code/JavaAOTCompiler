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
Control flow currently includes `if` and `while`; `while` is the only loop form.
The object model supports multiple classes, instance fields, reference values,
and allocation, but not constructors, inheritance, or instance methods.

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

An object example:

```java
class Node {
    int value;
    Node next;
}

class Main {
    static void main() {
        Node first = new Node();
        Node second = new Node();
        first.value = 42;
        first.next = second;
        second.value = 7;
        printInt(first.next.value);
        printBool(first.next.next == null);
    }
}
```

The expected result is:

```text
7
true
```

## 3. Program Structure

A JAOT0 source file contains one or more class declarations. Class names must be
unique within the program. A class may refer to another class declared later in
the same program.

```text
program
    ::= class-declaration+
```

A class declaration has the form:

```java
class ClassName {
}
```

Each class name also defines a reference type. A class declaration contains
instance fields followed by static methods.

## 4. Classes

The basic syntax is:

```java
class Identifier {
    field*
    method*
}
```

Fields and methods have no access modifiers and are accessible from all
classes. Fields are instance fields, not static fields. A field declaration
has a type and a name and cannot have an initializer. Fields are initialized
to their type's default value when an object is allocated.

JAOT0 does **not** currently support:

- constructors
- inheritance
- interfaces
- nested classes
- generics
- annotations
- access modifiers
- static fields
- instance methods
- field initializers
- `this`

These belong to later language stages.

## 5. Methods

Methods are declared using:

```java
static ReturnType methodName(ParameterList) {
    statements
}
```

All methods are static. A method's parameter and return types may be `int`,
`boolean`, or a class reference type. Methods are called without an object
receiver.

## 6. Entry Point

Every executable JAOT0 program must provide:

```java
class Main {
    static void main() {
    }
}
```

The program must define exactly one `static void main()` method, and it must be
declared in the class named `Main`.

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

## 7.4 Class reference types

Each declared class name is also a reference type. A reference value is either
`null` or the identity of an object allocated from that class. Reference types
may be used for fields, local variables, method parameters, and method return
types.

```java
Node first = new Node();
Node next = null;
```

The `null` literal can be assigned, passed, or returned wherever a reference
type is expected. A non-null reference can only be assigned, passed, or
returned where the declared type is the same class type; there is no
inheritance or implicit conversion between reference types. `==` and `!=`
compare reference identity, not field contents. References of different
declared class types cannot be compared, but a reference can be compared with
`null`. `null == null` is `true` and `null != null` is `false`.

The `new ClassName()` expression allocates a distinct object of that class.
Every field is initialized to its default value: `0` for `int`, `false` for
`boolean`, and `null` for a reference type. Object allocation does not call a
constructor; constructors are not supported. Objects remain allocated for the
lifetime of the program; JAOT0 does not specify or require garbage collection.
If allocation fails, the program terminates abnormally.

Reading or writing a field through a `null` reference terminates the program
abnormally with a null-dereference runtime failure.

## 7.5 Future types

The following are intentionally reserved for later stages:

```text
long
char
arrays
String
```

## 8. Variables

Local variables of type `int`, `boolean`, or a declared class reference type can
be declared inside methods. They may be declared with or without an initializer:

```java
int answer;
int initialized = 42;
boolean enabled = true;
Node next = null;
```

A variable declared without an initializer must be assigned before it is
read. Reading it earlier is a semantic error. Method parameters are initialized
when the method is called. Each block introduces a scope. A local variable is
visible from its declaration to the end of its containing block, including
nested blocks; a nested block may declare a variable with the same name as one
in an enclosing block.

## 9. Assignment

Local variables, method parameters, and object fields are mutable. An
assignment updates an existing target with a value of the same type; it does
not declare a new variable. Assignment is a statement, not an expression:

```java
int count = 0;
count = count + 1;
```

An assignment target is either a previously declared parameter or local
variable, or a field selected from an object reference. Assignment does not
declare a new variable. Global variables and static fields are not supported.

```text
variable-declaration:
    value-type Identifier ("=" expression)? ;

assignment-statement:
    assignable = expression ;
```

## 10. Expressions

JAOT0 supports integer, boolean, and reference expressions.

```java
1 + 2
20 + 22
10 * 5
100 - 25
value >= 0
enabled == false
!enabled
(enabled && ready)
(value > 0 || enabled)
```

Expressions can contain:

- integer literals
- boolean literals
- `null`
- object allocation expressions
- variables
- field accesses
- function calls
- arithmetic, comparison, and logical operators
- parentheses

## 11. Operators

The initial operators are:

| Operator | Meaning               |
|----------|-----------------------|
| `+`      | addition              |
| `-`      | subtraction           |
| `*`      | multiplication        |
| `/`      | integer division      |
| `<`      | less than             |
| `<=`     | less than or equal    |
| `>`      | greater than          |
| `>=`     | greater than or equal |
| `==`     | equality              |
| `!=`     | inequality            |
| `!`      | logical negation      |
| `&&`     | logical and           |
| `\|\|`   | logical or            |

Integer division truncates toward zero. Dividing by zero terminates the
program abnormally. The otherwise-overflowing expression
`-2147483648 / -1` evaluates to `-2147483648`.

```java
int a = 10 + 5;
int b = 10 - 5;
int c = 10 * 5;
int d = 10 / 5;
boolean ok = (a > 0) && (b >= 0);
boolean flipped = !(ok || false);
```

Operator precedence, from highest to lowest:

```text
! 
* /
+ -
< <= > >=
== !=
&&
||
```

Operators at the same precedence level associate from left to right. Arithmetic
operators require `int` operands and produce an `int`. Relational operators
require `int` operands and produce a `boolean`. Equality operators require two
operands of the same type and produce a `boolean`: `int`, `boolean`, or the same
class reference type. A reference may also be compared with `null`. The
logical negation operator `!` requires a `boolean` operand and produces a
`boolean`. The logical operators `&&` and `||` require `boolean` operands and
produce a `boolean`.

Relational comparisons use signed integer values. Operands are evaluated from
left to right. The logical operators `&&` and `||` use Java-style short-circuit
evaluation: `&&` does not evaluate its right operand when the left operand is
`false`, and `||` does not evaluate its right operand when the left operand is
`true`.

## 12. Function Calls

Methods are static. A method in the current class may be called by its name; a
method in another class is called using its class name and a dot:

```text
methodName(argument0, argument1)
ClassName.methodName(argument0, argument1)
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

Method names must be unique within each class. A call must resolve to a declared
method, supply the declared number of arguments, and pass arguments of the
declared parameter types. Calls to methods in another class must use the
class-qualified form. Built-in runtime functions remain unqualified.

Arguments are expressions:

```java
printInt(20 + 22);
int x = add(10 * 2, 20 + 2);
Node node = new Node();
node.next = null;
int value = node.value;
```

## 13. Stack Frames and Calling Convention

The initial native backend targets x86-64 System V and uses a frame pointer for
each method. `int` values use signed 32-bit representation; `boolean` values
use `0` for `false` and `1` for `true`. Parameters occupy slots just like
locals; the method prologue copies incoming arguments into those slots so that
all variable references use the same frame-relative addressing. Integer and
boolean slots are four bytes; reference slots are eight bytes and store native
object addresses.

The frame layout grows down from `%rbp`, with slots assigned to parameters and
locals according to their types. Space for every slot is reserved once in the
method prologue. The total frame allocation is rounded up to a multiple of 16
bytes to preserve the required stack alignment for calls.

The prologue saves the caller's frame pointer, establishes the new `%rbp`, and
reserves the aligned frame allocation. Methods return `int` and `boolean`
results in `%eax`; boolean results use the same `0`/`1` representation.
Reference results are returned in `%rax`. The epilogue uses `leave` and `ret`
to restore the caller's frame and return address.

Arguments use the x86-64 System V integer argument registers in order: `%rdi`,
`%rsi`, `%rdx`, `%rcx`, `%r8`, and `%r9`. `int` and `boolean` arguments use
their 32-bit register portions; reference arguments use the full 64-bit
registers. The callee copies arguments into their parameter slots. JAOT0
currently supports at most six parameters per method; stack-passed arguments
are not implemented.

Call arguments are evaluated from left to right and temporarily pushed so
evaluating a later argument cannot overwrite an earlier result. The values are
then moved into the argument registers and the call is emitted. Temporary
expression pushes are tracked by the code generator, which adds eight bytes of
padding when needed so the stack is 16-byte aligned immediately before a call.
The temporary pushes and any call padding do not form part of the method's
fixed local-variable frame.

The calling convention and stack-frame layout follow the
[System V AMD64 ABI](https://gitlab.com/x86-psABIs/x86-64-ABI). The emitted
instruction syntax follows GNU `as`'s
[i386 syntactical conventions](https://sourceware.org/binutils/docs/as/i386_002dSyntax.html).
Instruction behavior is based on Intel's
[64 and IA-32 Software Developer Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html).

Object references to occupy eight-byte slots in JAOT0's initial x86-64 backend.
The slot stores a native object address; `null` is represented by zero. Object
fields use four bytes for `int` and `boolean` values and eight bytes for
references. The object representation is an implementation detail except for
these observable reference and default-value semantics.

## 14. Built-in Runtime Functions

JAOT0 provides a very small runtime interface.

Initially:

```java
printInt(int value)
printBool(boolean value)
```

`printInt` prints an integer followed by a newline. `printBool` prints either
`true` or `false` followed by a newline.

```java
printInt(42);
printBool(true);
printBool(false);
```

outputs

```text
42
true
false
```

`printInt` and `printBool` are not normal JAOT0 methods implemented by the program. They are supplied by the native runtime.

`printBool` accepts only a `boolean` operand; passing an `int` is a semantic error.

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
while statement
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

int count = 3;
while (count > 0) {
    printInt(count);
    count = count - 1;
}
```

An `if` statement evaluates its parenthesized condition once. The condition
must have type `boolean`; the `then` block executes when it is `true`, and the
optional `else` block executes when it is `false`. If there is no `else` and
the condition is false, execution continues after the `if`. Blocks may contain
nested `if` statements.

A `while` statement has the form `while (condition) { ... }`. Its condition
must have type `boolean` and is evaluated before every iteration. The body
executes when the condition is `true`; when it is `false`, execution continues
after the loop. The body is always a block and may contain nested blocks,
`if` statements, and `while` statements. `while` is the only supported loop
form; `for`, `do-while`, `break`, and `continue` are not supported.

Every variable read must be definitely initialized on every path reaching the
read. After an `if`/`else`, a variable is definitely initialized only if it is
initialized at the end of every branch that can continue. For an `if` without
an `else`, assignments made only in the `then` block do not make a variable
definitely initialized after the statement. Return analysis likewise accounts
for both branches; both branches of an `if`/`else` must return for that
statement to guarantee a return. A `while` body is checked using the definite
initialization state at loop entry; assignments made only in the body do not
make a variable definitely initialized after the loop, because the body may
execute zero times. A `while` statement does not guarantee a return, even when
its condition is the literal `true`. Flow analysis treats both `if` branches as
possible and treats a loop as possibly executing zero times, even when its
condition is a literal.

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
null
new
return
if
else
while
```

They cannot be used as identifiers.

## 21. Punctuation

JAOT0 uses:

```java
{ }
( )
;
,
.
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
    ::= class-declaration+ EOF ;
    
class-declaration
    ::= "class" identifier
        "{"
        field-declaration*
        method-declaration*
        "}" ;

field-declaration
    ::= value-type identifier ";" ;

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
      | identifier
      | "void";

value-type
    ::= "int"
      | "boolean"
      | identifier;

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
      | while-statement
      | return-statement;

variable-declaration
    ::= value-type identifier ("=" expression)? ";" ;

assignment-statement
    ::= assignable "=" expression ";" ;

assignable
    ::= identifier
      | postfix-expression "." identifier;

if-statement
    ::= "if" "(" expression ")" block
        ("else" block)? ;

while-statement
    ::= "while" "(" expression ")" block ;

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
      | postfix-expression ;

primary-expression
    ::= integer
      | "true"
      | "false"
      | "null"
      | "new" identifier "(" ")"
      | identifier
      | method-call
      | "(" expression ")" ;

postfix-expression
    ::= primary-expression ("." identifier)* ;
      
method-call
    ::= identifier
        "(" argument-list? ")"
      | identifier "." identifier
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
types described above. An identifier used as a type must name a declared class.
`void` is valid only as a method return type. A `postfix-expression` can be
used as an expression only when each selected member names a field of the
preceding expression's class type; method calls are static and use the
`method-call` forms above.
