; x86_64 linux system calls
; Copyright (c) 2023-2024, Kaimakan71 and Quark contributors.
; Provided under the BSD 3-Clause license.

global exit
exit:
	mov rax, 0x3c
	syscall
	hlt

global read
read:
	mov rax, 0x00
	syscall
	ret

global write
write:
	mov rax, 0x01
	syscall
	ret
