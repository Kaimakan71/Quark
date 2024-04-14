; x86_64 hyra program entry point
; Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
; Provided under the BSD 3-Clause license.

extern main
extern exit

section .text

global _start
_start:
	endbr64

	call main

	mov rdi, rax
	jmp exit
