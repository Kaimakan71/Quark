# Quark
Quark is a general-purpose compiled programming language intended for use in many environments, from kernels to games to embedded systems. Its syntax is most similar to C or C++, but also attempts to be more consistent and readable.

# Building the Compiler
Run `make`. Parallel builds can be run with `make -j<number of threads>`.
Run `make clean` to delete all binaries, except for `quarkc` (the compiler).
Run `make test` to run compiler tests (which are in the `test` directory).

# TODO
- [x] Variables as arguments (e.g. `write(0, "hi", length);`)
- [ ] Refactor string (initialized data) system
- [ ] Escape sequences in characters and strings (e.g. `"Hello\n"`)
- [ ] Initialized local variables (e.g. `uint x = 0;`)
- [ ] More kinds of assignment values (e.g. strings and numbers)
- [ ] Global variables/constants (e.g. `const uint8 debugLevel = 0x03;`)
