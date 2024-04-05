# Quark
Quark is a general-purpose compiled programming language intended for use in many environments, from kernels to games to embedded systems. Its syntax is most similar to C or C++, but also attempts to be more consistent and readable.

# Building the Compiler
Run `make`. Parallel builds can be run with `make -j<number of threads>`.
Run `make clean` to delete all binaries, except for `quarkc` (the compiler).
Run `make test` to run compiler tests (which are in the `test` directory).

# Usage
`./compiler/quarkc -i <input file> -o <output file>`. This will generate assembly code from the quark source code. If you want to assemble the program, you can use NASM `nasm <asm file> -f elf64 -o <output file>`.

# TODO
- [x] Variables as arguments (e.g. `write(0, "hi", length);`)
- [x] Initialized local variables (e.g. `uint x = strlen("Test!");`)
- [x] More kinds of assignment values (e.g. strings and numbers)
- [ ] Return statements
- [ ] If statements
- [ ] Refactor string (initialized data) system
- [ ] Escape sequences in characters and strings (e.g. `"Hello\n"`)
- [ ] Global variables/constants (e.g. `const uint8 debugLevel = 0x03;`)
