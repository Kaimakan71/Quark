# Compiler build configuration.
# Copyright (c) 2023-2024, Quinn Stephens.
# Provided under the BSD 3-Clause license.

EXENAME = quarkc
OFILES = \
	log.o hash.o hashmap.o \
	lexer/char_info.o lexer/keyword.o lexer/lexer.o \
	parser/ast.o parser/variable.o parser/type.o parser/value.o parser/statement.o parser/procedure.o parser/parser.o \
	main.o

CFLAGS = -Wall -Wextra -Iinclude
LDFLAGS =
NASMFLAGS = -f elf64

ifeq ($(ENABLE_DEBUG),1)
CFLAGS += -DENABLE_DEBUG
endif

TEST_NAMES = $(addprefix tests/,return call types)
TEST_OFILES = $(addsuffix .o,$(TEST_NAMES))
TEST_ASMFILES = $(addsuffix .asm,$(TEST_NAMES))
TEST_EXENAMES = $(addsuffix .elf,$(TEST_NAMES))

ifeq ($(TARGET_OS),hyra)
TEST_LD = ../../Hyra/cross/bin/x86_64-hyra-gcc
TEST_LDFLAGS = -pie -nostdlib
else
TEST_LD = $(LD)
TEST_LDFLAGS = -static -nostdlib
endif

.PHONY: all
all: $(EXENAME)

$(EXENAME): $(OFILES)
	@echo Linking $@...
	@$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	@echo Compiling $<...
	@$(CC) -c $< $(CFLAGS) -o $@

.PHONY: test
test: $(TEST_EXENAMES)

%.elf: %.o
	@echo Linking $@...
	@$(TEST_LD) $(TEST_LDFLAGS) ../lib/libquark.a $< -o $@

%.o: %.asm
	@echo Assembling $<
	@$(NASM) $< $(NASMFLAGS) -o $@

%.asm: %.quark
	@echo Compiling $<...
	@./$(EXENAME) -i $< -o $@

.PHONY: clean
clean:
	@echo Cleaning compiler...
	@rm -f $(OFILES) $(TEST_OFILES) $(TEST_ASMFILES) $(TEST_EXENAMES)
