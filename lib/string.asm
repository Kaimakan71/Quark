; x86_64 string functions
; Copyright (c) 2023-2024, Quinn Stephens.
; Provided under the BSD 3-Clause license.

section .text

global strlen
strlen:
	mov rax, rdi
.loop:
	mov bl, [rax]
	cmp bl, 0
	je .exit
	inc rax
	jmp .loop
.exit:
	sub rax, rdi
	ret
