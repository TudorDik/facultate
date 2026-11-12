section .note.GNU-stack

extern scanf
extern printf
extern atol

section .rodata
fmt_rpn_str: db "%63s", 0
fmt_rpn_out: db "%ld", 10, 0

section .text

global reverse_polish_notation

reverse_polish_notation:
    push rbp
    mov rbp, rsp
    xor rax, rax

    push r15
    push rbx
    sub rsp, 64                 ; buffer 64 bytes

.read_loop:
    mov r15, rsp
    and rsp, -16                ; aliniere 16
    lea rdi, [rel fmt_rpn_str]
    lea rsi, [rbp - 80]         ; buffer
    xor eax, eax
    call scanf
    mov rsp, r15

    cmp eax, 0                  ; EOF?
    jle .done

    ; operator daca strlen == 1
    mov al, byte [rbp - 80]
    cmp byte [rbp - 79], 0      ; al doilea byte == NUL?
    jne .is_number

    cmp al, '+'
    je .do_add
    cmp al, '-'
    je .do_sub
    cmp al, '*'
    je .do_mul
    cmp al, '/'
    je .do_div

.is_number:
    mov r15, rsp
    and rsp, -16                ; aliniere 16
    lea rdi, [rbp - 80]         ; buffer
    call atol
    mov rsp, r15

    push rax
    jmp .read_loop

.do_add:
    pop rbx
    pop rax
    add rax, rbx
    push rax
    jmp .read_loop

.do_sub:
    pop rbx
    pop rax
    sub rax, rbx
    push rax
    jmp .read_loop

.do_mul:
    pop rbx
    pop rax
    imul rax, rbx
    push rax
    jmp .read_loop

.do_div:
    pop rcx
    pop rax
    cqo
    idiv rcx
    push rax
    jmp .read_loop

.done:
    pop rsi

    mov r15, rsp
    and rsp, -16                ; aliniere 16
    lea rdi, [rel fmt_rpn_out]
    xor eax, eax
    call printf
    mov rsp, r15

    mov rbx, [rbp - 16]         ; restaurare rbx
    mov r15, [rbp - 8]          ; restaurare r15

    leave
    ret
