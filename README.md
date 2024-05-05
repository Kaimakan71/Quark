# Quark
Quark is a general-purpose compiled programming language intended for use in many environments, from kernels to games to embedded systems. Its syntax is most similar to C or C++, but also attempts to be more consistent and readable.

# Building the Compiler
Quark is built with clang and llvm, which must be installed before building.
Run `make`. Parallel builds can be run with `make -j<number of threads>`.
Run `make clean` to delete all binaries, except for `quarkc` (the compiler).
Run `make test` to run compiler tests (which are in the `test` directory).

# Usage
`./compiler/quarkc -i filename.quark -o filename.asm`. This will generate assembly code from the quark source code. If you want to assemble the program, you can use NASM `nasm filename.asm -f elf64 -o filename.o`.

# TODO
* Expression/condition parsing
* Escape sequences (e.g. `"Hi\n"`)
* Global variables/constants (e.g. `const uint8 debugLevel = 0x03;`)
