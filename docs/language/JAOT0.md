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

The initial implementation should use 32-bit signed integer semantics.

## 7.2 `void`

`void` means that a method does not return a value.

```java
static void hello() {
    printInt(42);
}
```

A `void` method cannot be used as an expression.

## 7.3 Future types

The following are intentionally reserved for later stages:

```text
long
boolean
char
reference types
arrays
String
```

## 8. Variables

Local variables can be declared inside methods.
Variables must be initialized when declared.

Therefore this is not valid JAOT0:
```java
int answer;
```

## 9. Assignment

The initial JAOT0 language may support assignment:

```java
int x = 42;
```

However, I recommend that the first JAOT0 subset initially only support initialization:

```java
int x = 42;
```

and postpone reassignment until the semantic/IR layer is ready.

That keeps the first compiler significantly simpler.

```text
variable declaration:
    int Identifier = expression ;
```

## 10. Expressions

JAOT0 supports integer expressions.

```java
1 + 2
20 + 22
10 * 5
100 - 25
```

Expressions can contain:

- integer literals
- variables
- function calls
- arithmetic operators
- parentheses

## 11. Arithmetic Operators

The initial operators are:

| Operator | Meaning |
| --- | --- |
| `+` | addition |
| `-` | subtraction |
| `*` | multiplication |
| `/` | integer division |

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
```

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
each method. Each parameter and local variable gets a four-byte stack slot,
matching the 32-bit `int` type. Parameters occupy slots just like locals; the
method prologue copies incoming arguments into those slots so that all variable
references use the same frame-relative addressing.

The frame layout grows down from `%rbp`: the first parameter is stored at
`-4(%rbp)`, the next parameter at `-8(%rbp)`, followed by local variables.
Space for every slot is reserved once in the method prologue. The total frame
allocation is rounded up to a multiple of 16 bytes to preserve the required
stack alignment for calls.

The prologue saves the caller's frame pointer, establishes the new `%rbp`, and
reserves the aligned frame allocation. Methods return their integer result in
`%eax`; the epilogue uses `leave` and `ret` to restore the caller's frame and
return address.

Integer arguments use the x86-64 System V registers in order: `%rdi`, `%rsi`,
`%rdx`, `%rcx`, `%r8`, and `%r9`. The callee copies the corresponding 32-bit
register values into its parameter slots. JAOT0 currently supports at most six
parameters per method; stack-passed arguments are not implemented.

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

A method returning a value uses:

```java
return expression;
```

A `void` method may use:

```java
return;
```

Every `int` method must return an `int` value on every reachable path. The
current JAOT0 version has no conditional or loop statements, so an `int` method
must contain a return statement; otherwise, execution could reach the end
without producing a value. A `void` method may either return with `return;` or
reach the end of its body.

## 16. Statements

The initial statement set is intentionally tiny:

```text
variable declaration
expression statement
return statement
```

```java
int x = 42;
printInt(x);
return x;
```

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

The initial syntax is:

```text
letter
letter_or_digit*
```

A digit cannot be the first character.

## 20. Keywords

Reserved keywords:

```java
class
static 
void
int
return
```

They cannot be used as identifiers.

## 21. Punctuation

JAOT0 uses:

```java
{ }
( )
;
,
```

```java
class Main {
    static int add(int a, int b) {
        return a + b;
    }
}
```

## 22. Complete Minimal Grammar

The initial JAOT0 grammar can be described as:

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
    ::= "int" identifier ;
    
type
    ::= "int"
      | "void";
      
block
    ::= "{"
        statement*
        "}";
        
statement
    ::= variable-declaration
      | expression-statement
      | return-statement;
      
variable-declaration
    ::= "int" identifier "=" expression ";" ;
    
expression-statement
    ::= expression ";" ;
    
return-statement
    ::= "return" expression? ";" ;
    
expression
    ::= addition ;
    
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
      | identifier
      | call
      | "(" expression ")" ;
      
call
    ::= identifier
        "(" argument-list? ")" ;
        
argument-list:
    ::= expression ("," expression)* ;
    
integer
    ::= digit+ ;
    
identifier:
    ::= letter (letter | digit)* ;
```

This grammar is intentionally small.
