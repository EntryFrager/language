global main

extern print

extern input

section .text

num: dq 1.1

main:
		push rbp
		mov rbp, rsp

		xor rax, rax
		xor r11, r11
		xor r12, r12
		xor r15, r15

		call input
		mov qword [ram + (r15 + 0) * 8], rax

		call input
		mov qword [ram + (r15 + 1) * 8], rax

		call input
		mov qword [ram + (r15 + 2) * 8], rax

		push qword [num]
		call print

		push qword [ram + (r15 + 0) * 8]
		pop qword [ram + (r15 + 3) * 8]

		push qword [ram + (r15 + 1) * 8]
		pop qword [ram + (r15 + 4) * 8]

		push qword [ram + (r15 + 2) * 8]
		pop qword [ram + (r15 + 5) * 8]

		push r15
		add r15, 3
		call solve_dispetcher

		pop r15
		push rax

		mov rsp, rbp
		pop rbp

		ret

solve_dispetcher:
		push rbp
		mov rbp, rsp

		push qword [ram + (r15 + 0) * 8]
		push 0
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		cmpneqsd xmm1, xmm0
		movq rax, xmm1
		test rax, rax
		jnz .if_1

		push qword [ram + (r15 + 1) * 8]
		pop qword [ram + (r15 + 3) * 8]

		push qword [ram + (r15 + 2) * 8]
		pop qword [ram + (r15 + 4) * 8]

		push r15
		add r15, 3
		call solve_linear

		pop r15
		push rax

		jmp .end_if_0
	.if_1:
		push qword [ram + (r15 + 0) * 8]
		pop qword [ram + (r15 + 3) * 8]

		push qword [ram + (r15 + 1) * 8]
		pop qword [ram + (r15 + 4) * 8]

		push qword [ram + (r15 + 2) * 8]
		pop qword [ram + (r15 + 5) * 8]

		push r15
		add r15, 3
		call solve_square

		pop r15
		push rax

	.end_if_0:
		push 0
		pop rax

		mov rsp, rbp
		pop rbp

		ret


solve_linear:
		push rbp
		mov rbp, rsp

		push 0
		push 1
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		subsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop qword [ram + (r15 + 2) * 8]

		push 1
		pop qword [ram + (r15 + 3) * 8]

		push 1000
		pop qword [ram + (r15 + 4) * 8]

		push qword [ram + (r15 + 0) * 8]
		push 0
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		cmpneqsd xmm1, xmm0
		movq rax, xmm1
		test rax, rax
		jnz .if_3

		push qword [ram + (r15 + 1) * 8]
		push 0
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		cmpneqsd xmm1, xmm0
		movq rax, xmm1
		test rax, rax
		jnz .if_5

		push qword [ram + (r15 + 4) * 8]
		call print

		jmp .end_if_4
	.if_5:
		push qword [ram + (r15 + 2) * 8]
		call print

	.end_if_4:
		jmp .end_if_2
	.if_3:
		push 0
		push qword [ram + (r15 + 1) * 8]
		push qword [ram + (r15 + 0) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		divsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		subsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop qword [ram + (r15 + 5) * 8]

		push qword [ram + (r15 + 3) * 8]
		call print

		push qword [ram + (r15 + 5) * 8]
		call print

	.end_if_2:
		push 0
		pop rax

		mov rsp, rbp
		pop rbp

		ret


solve_square:
		push rbp
		mov rbp, rsp

		push 0
		push 1
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		subsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop qword [ram + (r15 + 3) * 8]

		push 1
		pop qword [ram + (r15 + 4) * 8]

		push 2
		pop qword [ram + (r15 + 5) * 8]

		push qword [ram + (r15 + 2) * 8]
		push 0
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		cmpneqsd xmm1, xmm0
		movq rax, xmm1
		test rax, rax
		jnz .if_7

		push 0
		pop qword [ram + (r15 + 6) * 8]

		push 0
		push qword [ram + (r15 + 1) * 8]
		push qword [ram + (r15 + 0) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		divsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		subsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop qword [ram + (r15 + 7) * 8]

		push qword [ram + (r15 + 5) * 8]
		call print

		push qword [ram + (r15 + 6) * 8]
		call print

		push qword [ram + (r15 + 7) * 8]
		call print

		jmp .end_if_6
	.if_7:
		push qword [ram + (r15 + 1) * 8]
		push qword [ram + (r15 + 1) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		mulsd xmm1, xmm0
		movq r11, xmm1
		push r11
		push 4
		push qword [ram + (r15 + 0) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		mulsd xmm1, xmm0
		movq r11, xmm1
		push r11
		push qword [ram + (r15 + 2) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		mulsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		subsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop qword [ram + (r15 + 8) * 8]

		push qword [ram + (r15 + 8) * 8]
		push 0
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		cmplesd xmm1, xmm0
		movq rax, xmm1
		test rax, rax
		jnz .if_9

		pop r11
		movq xmm0, r11
		sqrtsd xmm0, xmm0
		movq r11, xmm0
		push r11
		pop qword [ram + (r15 + 9) * 8]

		push 0
		push qword [ram + (r15 + 1) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		subsd xmm1, xmm0
		movq r11, xmm1
		push r11
		push qword [ram + (r15 + 9) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		subsd xmm1, xmm0
		movq r11, xmm1
		push r11
		push 2
		push qword [ram + (r15 + 0) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		mulsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		divsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop qword [ram + (r15 + 6) * 8]

		push 0
		push qword [ram + (r15 + 1) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		subsd xmm1, xmm0
		movq r11, xmm1
		push r11
		push qword [ram + (r15 + 9) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		addsd xmm1, xmm0
		movq r11, xmm1
		push r11
		push 2
		push qword [ram + (r15 + 0) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		mulsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		divsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop qword [ram + (r15 + 7) * 8]

		push qword [ram + (r15 + 5) * 8]
		call print

		push qword [ram + (r15 + 6) * 8]
		call print

		push qword [ram + (r15 + 7) * 8]
		call print

		jmp .end_if_8
	.if_9:
		push qword [ram + (r15 + 8) * 8]
		push 0
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		cmpneqsd xmm1, xmm0
		movq rax, xmm1
		test rax, rax
		jnz .if_10

		push 0
		push qword [ram + (r15 + 1) * 8]
		push 2
		push qword [ram + (r15 + 0) * 8]
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		mulsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		divsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop r11
		pop r12
		movq xmm0, r11
		movq xmm1, r12
		subsd xmm1, xmm0
		movq r11, xmm1
		push r11
		pop qword [ram + (r15 + 10) * 8]

		push qword [ram + (r15 + 4) * 8]
		call print

		push qword [ram + (r15 + 10) * 8]
		call print

		jmp .end_if_8
	.if_10:
		push qword [ram + (r15 + 3) * 8]
		call print

	.end_if_8:
	.end_if_6:
		push 0
		pop rax

		mov rsp, rbp
		pop rbp

		ret

section .data

ram: times 10000 dq 0
