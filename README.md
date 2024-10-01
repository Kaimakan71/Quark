# Quark

Quark is a general-purpose compiled programming language intended for use in many environments, from kernels to games to embedded systems. Its syntax is most similar to C or C++, but also attempts to be more consistent and readable.

# Try it!

Quark is built with Clang and LLVM, which must be installed before building.
Run `make`. Parallel builds can be run with `make -j<number of threads>`.
Run `make clean` to delete generated binaries.
Run `make test` to run compiler tests (which are in the `test` directory).

`./compiler/quarkc -i filename.quark -o filename.asm`. This will generate assembly code from the quark source code. If you want to assemble the program, you can use NASM `nasm filename.asm -f elf64 -o filename.o`.

If you want to disable debug messages, clean (`make clean`) then rebuild (`make ENABLE_DEBUG=0`).

# Why Make Another Language?

There are many languages out there that can be used to write great software. Languages like C or C++ have similar feature sets and work just fine, so why not just use an existing language? The problems lie in the compiler and clutter that come with the language. The GCC/Binutils toolchain is extremely bloated, and it consumes lots of resources. Alternatives like Clang/LLVM are better, but the C language still has inconsistencies that just make it harder to learn and understand. Over time, it has coalesced many features like the preprocessor that make everything slower and more confusing. Quark code should compile fast, run fast, and be easily understood.

# Is this a Serious Project?

There are many compiler projects out there, but unfortunately many of them either don't get finished or don't actually get used even by their creators. Quark is supposed to be a *real* language and if possible, I'd like to use it in all of my future projects. This project is not just about learning compiler design or experimenting with parsers, it's about improving the development experience for future programmers.
