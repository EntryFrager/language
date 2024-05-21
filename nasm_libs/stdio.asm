global print


global input


extern printf


extern scanf


section .data


format_in: db "%lf", 0


format_out: db "%lg", 0Dh, 0Ah, 0


value: dq 0


section .text


print:
    pop r15

    movsd xmm0, [rsp]
    add rsp, 8

    push r15
    push rbp

    mov rax, 1
    mov rdi, format_out

    call printf

    pop rbp
    ret


input:
    push rbp

    mov rsi, value
    mov rdi, format_in

    call scanf

    mov rax, qword [value]

    pop rbp
    ret


