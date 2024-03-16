# Compiler build configuration.
# Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
# Provided under the BSD 3-Clause license.
BINARY = quarkc
OFILES = error.o hash.o lexer/char_info.o lexer/keyword.o lexer/lexer.o parser/ast.o parser/parser.o codegen/codegen.o main.o
CFLAGS = -I include
LDFLAGS =

CC ?= cc
LD ?= ld

.PHONY: all test clean

all: $(BINARY)

$(BINARY): $(OFILES)
	@echo Linking $@...
	@$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	@echo Compiling $<...
	@$(CC) -c $< $(CFLAGS) -o $@

test:
	@echo Running compiler tests...
	@./$(BINARY) -c tests/exit.quark -o tests/exit.asm
	@./$(BINARY) -c tests/write.quark -o tests/write.asm

clean:
	@echo Cleaning compiler...
	@rm -f $(OFILES) tests/exit.asm tests/write.asm
