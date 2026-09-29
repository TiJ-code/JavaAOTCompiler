# JAOT - Java Ahead Of Time

JAOT is a Java ahead-of-time compiler writtin in Java.

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
