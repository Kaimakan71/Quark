# Compiler build configuration.
# Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
# Provided under the BSD 3-Clause license.
BINARY = quarkc
OFILES = error.o hash.o lexer/char_info.o lexer/keyword.o lexer/lexer.o parser/ast.o parser/type.o parser/parser.o codegen/codegen.o main.o
CFLAGS = -I include
LDFLAGS =

TEST_OFILES = tests/exit.o tests/write.o
LIB_OFILES = ../lib/entry.o ../lib/system.o ../lib/string.o

CC ?= cc
LD ?= ld
NASM ?= nasm

.PHONY: all test clean

all: $(BINARY)

$(BINARY): $(OFILES)
	@echo Linking $@...
	@$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	@echo Compiling $<...
	@$(CC) -c $< $(CFLAGS) -o $@

test: tests/exit.elf tests/write.elf tests/assignment.elf

%.elf: %.o
	@echo Linking $@...
	@$(LD) $(LIB_OFILES) $< -o $@

%.o: %.asm
	@echo Assembling $<
	@$(NASM) -g $< -f elf64 -o $@

%.asm: %.quark
	@echo Compiling $<...
	@./$(BINARY) -i $< -o $@

clean:
	@echo Cleaning compiler...
	@rm -f $(OFILES) $(TEST_OFILES) $(TEST_OFILES:.o=.asm) $(TEST_OFILES:.o=.elf)