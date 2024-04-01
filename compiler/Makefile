# Compiler build configuration.
# Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
# Provided under the BSD 3-Clause license.
BINARY = quarkc
OFILES = error.o hash.o lexer/char_info.o lexer/keyword.o lexer/lexer.o parser/ast.o parser/type.o parser/parser.o codegen/codegen.o main.o
CFLAGS = -I include
LDFLAGS =

CC ?= cc
LD ?= ld
NASM ?= nasm

TESTS_TARGET = linux
TESTS_OFILES = tests/exit.o tests/assignment.o tests/write.o tests/file.o
LIB_OFILES = ../lib/$(TESTS_TARGET)/entry.o ../lib/$(TESTS_TARGET)/system.o ../lib/string.o

ifeq ($(TESTS_TARGET),hyra)
TESTS_LD = ../../Hyra/cross/bin/x86_64-hyra-gcc
TESTS_LDFLAGS = -pie -nostdlib
else
TESTS_LD = $(LD)
TESTS_LDFLAGS = -static -nostdlib
endif

.PHONY: all test clean

all: $(BINARY)

$(BINARY): $(OFILES)
	@echo Linking $@...
	@$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	@echo Compiling $<...
	@$(CC) -c $< $(CFLAGS) -o $@

test: tests/exit.elf tests/assignment.elf tests/write.elf tests/file.elf

%.elf: %.o
	@echo Linking $@...
	@$(TESTS_LD) $(TESTS_LDFLAGS) $(LIB_OFILES) $< -o $@

%.o: %.asm
	@echo Assembling $<
	@$(NASM) $< -f elf64 -o $@

%.asm: %.quark
	@echo Compiling $<...
	@./$(BINARY) -i $< -o $@

clean:
	@echo Cleaning compiler...
	@rm -f $(OFILES) $(TESTS_OFILES) $(TESTS_OFILES:.o=.asm) $(TESTS_OFILES:.o=.elf)
