# Compiler build configuration.
# Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
# Provided under the BSD 3-Clause license.
ifeq ($(OS), Windows_NT)
BINARY = quarkc.exe
else
BINARY = quarkc
endif
BINARY = quarkc
OFILES = src/error.o src/char_info.o src/hash.o src/lexer.o src/parser.o src/interperter.o src/main.o
CFLAGS = -I include
LDFLAGS =

CC ?= cc
LD ?= ld

.PHONY: all
all: $(BINARY)

$(BINARY): $(OFILES)
	@echo Linking $@...
	@$(CC) $^ $(LDFLAGS) -o $@

%.o: %.c
	@echo Compiling $<...
	@$(CC) -c $< $(CFLAGS) -o $@

.PHONY: test
test:
	@echo "Running test..."
	@./$(BINARY) test.quark

.PHONY: clean
clean:
	@echo Cleaning...
	@rm -f $(OFILES)
