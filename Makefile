# Build configuration.
# Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
# Provided under the BSD 3-Clause license.

export ENABLE_DEBUG ?= 1
export CC ?= clang
export LD ?= lld
export NASM ?= nasm
export TARGET_OS ?= linux

.PHONY: all
all:
	@$(MAKE) -C compiler
	@$(MAKE) -C lib

.PHONY: test
test:
	@$(MAKE) -C compiler test

.PHONY: clean
clean:
	@$(MAKE) -C compiler clean
	@$(MAKE) -C lib clean
