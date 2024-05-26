# Compiler build configuration.
# Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
# Provided under the BSD 3-Clause license.
BINARY = quarkc
CFLAGS = -I include
ifeq ($(ENABLE_DEBUG),1)
CFLAGS += -DENABLE_DEBUG
endif
LDFLAGS =
NASMFLAGS = -f elf64

OFILES = error.o hash.o \
	lexer/char_info.o lexer/keyword.o lexer/lexer.o \
	parser/ast.o parser/storage.o parser/type.o parser/value.o parser/statement.o parser/procedure.o parser/parser.o \
	codegen/codegen.o \
	main.o

TESTS = $(addprefix tests/,exit write file if)
TESTS_OFILES = $(addsuffix .o,$(TESTS))
TESTS_ASMFILES = $(addsuffix .asm,$(TESTS))
TESTS_BINARIES = $(addsuffix .elf,$(TESTS))

ifeq ($(TARGET_OS),hyra)
TESTS_LD = ../../Hyra/cross/bin/x86_64-hyra-gcc
TESTS_LDFLAGS = -pie -nostdlib
else
TESTS_LD = $(LD)
TESTS_LDFLAGS = -static -nostdlib
endif

.PHONY: all test clean

.PHONY: all
all: $(BINARY)

$(BINARY): $(OFILES)
	@echo Linking $@...
	@$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	@echo Compiling $<...
	@$(CC) -c $< $(CFLAGS) -o $@

.PHONY: test
test: $(TESTS_BINARIES)

%.elf: %.o
	@echo Linking $@...
	@$(TESTS_LD) $(TESTS_LDFLAGS) ../lib/libquark.a $< -o $@

%.o: %.asm
	@echo Assembling $<
	@$(NASM) $< $(NASMFLAGS) -o $@

%.asm: %.quark
	@echo Compiling $<...
	@./$(BINARY) -i $< -o $@

.PHONY: clean
clean:
	@echo Cleaning compiler...
	@rm -f $(OFILES) $(TESTS_OFILES) $(TESTS_ASMFILES) $(TESTS_BINARIES)
