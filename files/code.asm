global main

extern print

extern input

extern sqrt_int

section .text


;------------------------------------------------------------------------------
;-----------------------------------Function-----------------------------------
;------------------------------------------------------------------------------


main:
		push rbp
		mov rbp, rsp

		xor rax, rax
		xor r11, r11
		xor r12, r12
		xor r15, r15

;--------------------Input--------------------

		call input
		mov qword [ram + r15 + 0 * 8], rax

;------------------End-Input------------------

;--------------------Expression--------------------

		push 1
		pop qword [ram + r15 + 1 * 8]

;------------------End-Expression------------------

;-------------------------If-Condition-------------------------

		push qword [ram + r15 + 0 * 8]
		push 0
		pop r11
		pop r12
		cmp r12, r11
		jb .if_2

		push qword [ram + r15 + 0 * 8]
		push 0
		pop r11
		pop r12
		cmp r12, r11
		jne .if_1

	.if_2:

;-----------------------End-If-Condition-----------------------

;-------------------------If-Body-------------------------

;--------------------Print--------------------

		mov rsi, qword [ram + r15 + 1 * 8]
		call print

;------------------End-Print------------------

;-----------------------End-If-Body-----------------------
		jmp .end_if_0
	.if_1:

;-------------------------If-Body-------------------------

;--------------------Expression--------------------


;-------------------------Call-Function-------------------------

		push qword [ram + r15 + 0 * 8]
		pop qword [ram + r15 + 3 * 8]

		push r15
		shr r15, 3
		add r15, 3
		shl r15, 3
		call fact

		pop r15
		push rax

;-----------------------End-Call-Function-----------------------
		pop qword [ram + r15 + 2 * 8]

;------------------End-Expression------------------

;--------------------Print--------------------

		mov rsi, qword [ram + r15 + 2 * 8]
		call print

;------------------End-Print------------------

;-----------------------End-If-Body-----------------------
	.end_if_0:
		pop rbp
		ret


;------------------------------------------------------------------------------
;-----------------------------------Function-----------------------------------
;------------------------------------------------------------------------------


fact:
		push rbp
		mov rbp, rsp

;-------------------------If-Condition-------------------------

		push qword [ram + r15 + 0 * 8]
		push 1
		pop r11
		pop r12
		cmp r12, r11
		jne .if_4


;-----------------------End-If-Condition-----------------------

;-------------------------If-Body-------------------------

;--------------------Return--------------------

		push qword [ram + r15 + 0 * 8]
		pop rax

		pop rbp

		ret


;-----------------------End-If-Body-----------------------
		jmp .end_if_3
	.if_4:
	.end_if_3:

;--------------------Expression--------------------

		push qword [ram + r15 + 0 * 8]
		push 1
		pop r11
		pop r12
		sub r12, r11
		push r12
		pop qword [ram + r15 + 1 * 8]

;------------------End-Expression------------------

;--------------------Expression--------------------

		push qword [ram + r15 + 0 * 8]

;-------------------------Call-Function-------------------------

		push qword [ram + r15 + 1 * 8]
		pop qword [ram + r15 + 2 * 8]

		push r15
		shr r15, 3
		add r15, 2
		shl r15, 3
		call fact

		pop r15
		push rax

;-----------------------End-Call-Function-----------------------
		pop r11
		pop rax
		imul r11
		push rax
		pop qword [ram + r15 + 0 * 8]

;------------------End-Expression------------------

;--------------------Return--------------------

		push qword [ram + r15 + 0 * 8]
		pop rax

		pop rbp

		ret

section .data

ram: times 10000 dq 0
