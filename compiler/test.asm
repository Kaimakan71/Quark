	section .text

	global main
	align 16
main:
	push rbp
	mov rbp, rsp
	xor rax, rax
	pop rbp
	ret
