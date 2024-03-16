# Build configuration.
# Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
# Provided under the BSD 3-Clause license.

CC ?= cc
LD ?= ld

.PHONY: all test clean

all:
	@$(MAKE) -C compiler
	@$(MAKE) -C lib

test:
	@$(MAKE) -C compiler test

clean:
	@$(MAKE) -C compiler clean
	@$(MAKE) -C lib clean
