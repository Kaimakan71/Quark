; x86_64 linux program entry point
; Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
; Provided under the BSD 3-Clause license.

global _start
extern main
extern exit
_start:
	call main

	mov rdi, rax
	call exit
