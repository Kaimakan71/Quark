; x86_64 hyra system calls
; Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
; Provided under the BSD 3-Clause license.

section .text

global exit
exit:
	mov rax, 0x01
	syscall
	hlt

global read
read:
	mov rax, 0x05
	syscall
	ret

global write
write:
	mov rax, 0x02
	syscall
	ret
