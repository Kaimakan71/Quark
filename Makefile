# Build configuration.
# Copyright (c) 2023-2024, Quinn Stephens.
# Provided under the BSD 3-Clause license.

export TARGET_OS = linux
export ENABLE_DEBUG = 1
export CC = clang
export LD = ld.lld

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
