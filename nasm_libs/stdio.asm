global print


global input


global sqrt_int


extern printf


extern scanf


section .data


format_in: db "%d", 0


format_out: db "%d", 0Dh, 0Ah, 0


value: dq 0


square_root: dq 0x00


section .text


print:
    push rbp
    mov rbp, rsp

    mov rax, 0
    mov rdi, format_out

    call printf

    mov rsp, rbp
    pop rbp

    ret


input:
    push rbp
    mov rbp, rsp

    mov rsi, value
    mov rdi, format_in

    call scanf

    mov rax, qword [value]

    mov rsp, rbp
    pop rbp

    ret


sqrt_int:
        push    rbp
        mov     rbp, rsp

        cvtsi2sd xmm0, rsi
        mov     rdx, square_root
        movsd   QWORD [rdx], xmm0

        fld    QWORD [square_root]
        fsqrt
        fstp   QWORD [square_root]

        movsd  xmm0, QWORD [square_root]
        cvtsd2si rax, xmm0

        pop     rbp
        ret


