# Standard library build configuration.
# Copyright (c) 2023-2024, Quinn Stephens.
# Provided under the BSD 3-Clause license.
BINARY = lib.o
LDFLAGS = -r
NASMFLAGS = -f elf64

OFILES = string.o
ifeq ($(TARGET_OS),hyra)
OFILES += hyra/entry.o hyra/system.o
else
OFILES += linux/entry.o linux/system.o
endif

.PHONY: all
all: $(BINARY)

$(BINARY): $(OFILES)
	@echo Linking $@...
	@$(LD) $^ $(LDFLAGS) -o $@

%.o: %.asm
	@echo Assembling $<...
	@nasm $< $(NASMFLAGS) -o $@

.PHONY: clean
clean:
	@echo Cleaning lib...
	@rm -f $(OFILES)
