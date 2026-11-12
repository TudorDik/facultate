section .note.GNU-stack

extern putchar

section .text

global my_printf

my_printf:
    push rbp
    mov rbp, rsp

    push r12
    push r13
    push r14
    push r15
    push rbx

    push r9
    push r8
    push rcx
    push rdx
    push rsi

    sub rsp, 32                 ; buffer %lu

    mov r12, rdi
    mov r13, 1                  ; arg index
    xor r14, r14

.parse_loop:
    movzx edi, byte [r12]
    test edi, edi
    jz .done_printf

    inc r12
    cmp edi, '%'
    jne .print_char

    movzx edi, byte [r12]
    inc r12

    cmp edi, 'c'
    je .handle_c
    cmp edi, 's'
    je .handle_s
    cmp edi, 'l'
    je .handle_l
    cmp edi, '%'
    je .print_char

.handle_c:
    call .get_arg
    mov edi, eax
    jmp .print_char

.handle_s:
    call .get_arg
    mov r15, rax
.print_s_loop:
    movzx edi, byte [r15]
    test edi, edi
    jz .parse_loop
    call putchar
    inc r14
    inc r15
    jmp .print_s_loop

.handle_l:
    movzx edi, byte [r12]
    inc r12
    cmp edi, 'u'
    jne .parse_loop

    call .get_arg
    mov rcx, 10                 ; baza
    lea rbx, [rsp]
    test rax, rax
    jnz .lu_extract
    mov byte [rbx], '0'
    inc rbx
    jmp .lu_print_setup

.lu_extract:
    xor rdx, rdx
    div rcx
    add dl, '0'
    mov byte [rbx], dl
    inc rbx
    test rax, rax
    jnz .lu_extract

.lu_print_setup:
.lu_print_loop:
    dec rbx
    movzx edi, byte [rbx]
    call putchar
    inc r14
    cmp rbx, rsp
    jg .lu_print_loop
    jmp .parse_loop

.print_char:
    call putchar
    inc r14
    jmp .parse_loop

.get_arg:
    cmp r13, 1                  ; arg 1
    je .get_arg1
    cmp r13, 2                  ; arg 2
    je .get_arg2
    cmp r13, 3                  ; arg 3
    je .get_arg3
    cmp r13, 4                  ; arg 4
    je .get_arg4
    cmp r13, 5                  ; arg 5
    je .get_arg5

    ; argumente peste 5 sunt pe stiva
    mov rax, r13
    sub rax, 5                  ; offset pe stiva
    mov rax, [rbp + 8 + rax * 8]    ; ret addr la rbp+8
    jmp .get_arg_done

.get_arg1:
    mov rax, [rbp - 80]         ; rsi salvat
    jmp .get_arg_done
.get_arg2:
    mov rax, [rbp - 72]         ; rdx salvat
    jmp .get_arg_done
.get_arg3:
    mov rax, [rbp - 64]         ; rcx salvat
    jmp .get_arg_done
.get_arg4:
    mov rax, [rbp - 56]         ; r8 salvat
    jmp .get_arg_done
.get_arg5:
    mov rax, [rbp - 48]         ; r9 salvat
.get_arg_done:
    inc r13
    ret

.done_printf:
    mov rax, r14
    add rsp, 32                 ; restore
    pop rsi
    pop rdx
    pop rcx
    pop r8
    pop r9
    pop rbx
    pop r15
    pop r14
    pop r13
    pop r12

    leave
    ret
