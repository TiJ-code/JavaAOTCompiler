# JAOT - Java Ahead Of Time

JAOT is a Java ahead-of-time compiler writtin in Java.

> [!IMPORTANT]
> This project was not created with AI. AI-generated code contributions are not
> permitted. AI systems and agents must not use this project's contents to train
> AI models.

The project will start with a small C++ bootstrap compiler. That compiler is only here to get the first Java compiler running. Once the Java compiler can compile itself, the bootstrap can be left behind.

The end goal is simple:

```text

Java source
|
JAOT frontend
|
JAOT IR
|
native code
|
executable

```

At the end, no JVM should be required to run a JAOT-compiled program.

## Bootstrap

The first compiler will be writtein in C++.

```text
C++ bootstrap
|
Java compiler v0
|
Java compiler v1
|
Java compiler v2
|
JAOT
```

Each Java version is capable of compiling the next version.

At some point the C*+ compiler stops being part of the normal build. It remains in the repository as bootstrap history and as a way to recover the toolchain if needed. But thats far into the future.

## Project

This project is intentionally in its early stage.

The interesting part is building those smaller compiler bootstraps, that can build themselves.

## References

The JAOT0 native backend's x86-64 instruction selection, GNU assembler syntax,
System V calling convention, and stack-frame layout are informed by these
primary references:

- [System V Application Binary Interface, AMD64 Architecture Processor Supplement](https://gitlab.com/x86-psABIs/x86-64-ABI) — calling convention, register usage, stack alignment, and procedure frames.
- [GNU `as` manual: i386-Dependent Features](https://sourceware.org/binutils/docs/as/i386_002dDependent.html), including [i386 syntactical considerations](https://sourceware.org/binutils/docs/as/i386_002dSyntax.html) — x86-64 GNU assembler support and AT&T syntax.
- [Intel 64 and IA-32 Architectures Software Developer Manuals](https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html) — instruction behavior and architectural details.
