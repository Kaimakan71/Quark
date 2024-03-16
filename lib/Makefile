# Standard library build configuration.
# Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
# Provided under the BSD 3-Clause license.
OFILES = entry.o system.o
NASMFLAGS = -f elf64

NASM ?= nasm

.PHONY: all clean

all: $(OFILES)

%.o: %.asm
	@echo Assembling $<...
	@nasm $< $(NASMFLAGS) -o $@

clean:
	@echo Cleaning lib...
	@rm -f $(OFILES)
