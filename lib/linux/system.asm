; x86_64 linux system calls
; Copyright (c) 2023-2024, Quinn Stephens.
; Provided under the BSD 3-Clause license.

section .text

global read
read:
	xor rax, rax
	syscall
	ret

global write
write:
	mov rax, 0x01
	syscall
	ret

global open
open:
	mov rax, 0x02
	syscall
	ret

global close
close:
	mov rax, 0x03
	syscall
	ret

global exit
exit:
	mov rax, 0x3c
	syscall
	hlt
