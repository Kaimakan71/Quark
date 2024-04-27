# Build configuration.
# Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
# Provided under the BSD 3-Clause license.

export CC ?= clang
export LD ?= clang
export NASM ?= nasm
export TESTS_TARGET ?= linux

.PHONY: all test clean

all:
	@$(MAKE) -C compiler
	@$(MAKE) -C lib

test:
	@$(MAKE) -C compiler test

clean:
	@$(MAKE) -C compiler clean
	@$(MAKE) -C lib clean
