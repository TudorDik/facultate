section .note.GNU-stack

extern printf

section .data
str_heavy:     db "The number is heavy", 10, 0
str_not_heavy: db "The number is not heavy", 10, 0
fmt_int:       db "%d", 10, 0

section .text

global ave
global switch_cases
global heavy
global flat_matrix


ave:
    push rbp
    mov rbp, rsp

    mov r8d, edx
    xor rcx, rcx

.loop_ave:
    movzx eax, byte [rdi + rcx]
    test eax, eax
    jz .done_ave

    sub eax, 'A'
    add eax, r8d

    cdq
    mov r9d, 31                 ; mod 31
    idiv r9d

    mov eax, edx
    add eax, 'A'
    mov byte [rsi + rcx], al

    inc rcx
    jmp .loop_ave

.done_ave:
    mov byte [rsi + rcx], 0     ; NUL
    leave
    ret


switch_cases:
    push rbp
    mov rbp, rsp

    xor rcx, rcx

.loop_sc:
    movzx eax, byte [rdi + rcx]
    test eax, eax
    jz .done_sc

    cmp eax, 'A'
    jl .check_lower
    cmp eax, 'Z'
    jg .check_lower
    add eax, 'a' - 'A'
    jmp .write_sc

.check_lower:
    cmp eax, 'a'
    jl .write_sc
    cmp eax, 'z'
    jg .write_sc
    sub eax, 'a' - 'A'

.write_sc:
    mov byte [rsi + rcx], al
    inc rcx
    jmp .loop_sc

.done_sc:
    mov byte [rsi + rcx], 0     ; NUL
    leave
    ret


heavy:
    push rbp
    mov rbp, rsp

    test edi, edi               ; MSB?
    jns .not_heavy

    mov eax, edi
    shr eax, 16                 ; bits 16-31
    xchg al, ah
    movzx eax, ax
    cmp eax, 255                ; > 1 byte?
    jle .not_heavy

    lea rdi, [rel str_heavy]
    xor eax, eax
    call printf
    leave
    ret

.not_heavy:
    lea rdi, [rel str_not_heavy]
    xor eax, eax
    call printf
    leave
    ret


flat_matrix:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8                  ; aliniere

    mov rbx, rdi
    mov r12d, esi

    xor r13d, r13d              ; j = 0

.col_loop:
    cmp r13d, r12d
    jge .done_fm

    xor r14d, r14d              ; i = 0
    mov r15d, 0x80000000        ; max = INT_MIN

.row_loop:
    cmp r14d, r12d
    jge .col_done

    mov eax, r14d
    imul eax, r12d
    add eax, r13d
    movsxd rax, eax

    mov edx, dword [rbx + rax*4]    ; offset 4
    cmp edx, r15d
    jle .skip_max
    mov r15d, edx
.skip_max:
    inc r14d
    jmp .row_loop

.col_done:
    lea rdi, [rel fmt_int]
    mov esi, r15d
    xor eax, eax
    call printf

    inc r13d
    jmp .col_loop

.done_fm:
    add rsp, 8                  ; restore
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx

    leave
    ret
