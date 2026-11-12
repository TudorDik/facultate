section .note.GNU-stack

extern malloc
extern realloc
extern free
extern printf

len_offset   equ 8       ; offset len
cap_offset   equ 12      ; offset cap
header_size  equ 16      ; sizeof struct
int_size     equ 4       ; sizeof int
log2_int     equ 2       ; log2 int_size
err_ret      equ -1      ; cod eroare
pad8         equ 8       ; padding aliniere

section .data
    ; mesaje pentru afisare; 10 = '\n', 0 = NUL
    err_len_msg   db "Error: len <= %d", 10, 0
    err_empty_msg db "The vector is empty", 10, 0  ; mesaj eroare
    fmt_start     db "v -> {(", 0  ; prefix
    fmt_elem      db "[%d]", 0     ; element ocupat
    fmt_empty     db "[]", 0       ; slot gol
    fmt_end       db "), %d, %d}", 10, 0  ; sufix

section .text

global new_vector
global set_element
global get_element
global push_element
global pop_element
global print_vector
global free_vector

new_vector:
    push rbp
    mov rbp, rsp
    xor rax, rax

    push rbx
    sub rsp, pad8

    mov ebx, edi
    mov edi, header_size
    call malloc
    test rax, rax
    jz .end_new

    mov dword [rax + len_offset], 0     ; len = 0
    mov dword [rax + cap_offset], ebx
    mov [rsp], rax

    mov edi, ebx
    shl edi, log2_int
    call malloc

    mov rbx, [rsp]
    mov [rbx], rax
    mov rax, rbx

.end_new:
    add rsp, pad8
    pop rbx

    leave
    ret

set_element:
    push rbp
    mov rbp, rsp
    xor rax, rax

    mov ecx, dword [rdi + len_offset]
    cmp edx, ecx
    jl .valid_set

    push rdi
    push rsi
    push rdx
    sub rsp, pad8

    lea rdi, [rel err_len_msg]
    mov esi, edx
    xor eax, eax
    call printf

    add rsp, pad8
    pop rdx
    pop rsi
    pop rdi
    mov rax, err_ret
    jmp .end_set

.valid_set:
    mov rax, [rdi]
    movsxd rcx, edx
    mov dword [rax + rcx * int_size], esi
    movsxd rax, edx

.end_set:
    leave
    ret

get_element:
    push rbp
    mov rbp, rsp
    xor rax, rax

    mov ecx, dword [rdi + len_offset]
    cmp esi, ecx
    jl .valid_get

    push rdi
    push rsi

    lea rdi, [rel err_len_msg]
    xor eax, eax
    call printf

    pop rsi
    pop rdi
    mov rax, err_ret
    jmp .end_get

.valid_get:
    mov rax, [rdi]
    movsxd rcx, esi
    mov eax, dword [rax + rcx * int_size]
    cdqe

.end_get:
    leave
    ret

push_element:
    push rbp
    mov rbp, rsp
    xor rax, rax

    push rbx
    push r12

    mov rbx, rdi
    mov r12d, esi

    mov ecx, dword [rbx + len_offset]
    mov edx, dword [rbx + cap_offset]
    cmp ecx, edx
    jl .skip_realloc

    shl edx, 1                  ; cap *= 2
    mov dword [rbx + cap_offset], edx

    mov rdi, [rbx]
    mov esi, edx
    shl esi, log2_int
    call realloc
    mov [rbx], rax

.skip_realloc:
    mov rax, [rbx]
    mov ecx, dword [rbx + len_offset]
    movsxd rdx, ecx
    mov dword [rax + rdx * int_size], r12d

    inc ecx
    mov dword [rbx + len_offset], ecx
    mov rax, rdx

    pop r12
    pop rbx

    leave
    ret

pop_element:
    push rbp
    mov rbp, rsp
    xor rax, rax

    mov ecx, dword [rdi + len_offset]
    cmp ecx, 0                  ; vector gol?
    jg .valid_pop

    push rdi
    sub rsp, pad8

    lea rdi, [rel err_empty_msg]
    xor eax, eax
    call printf

    add rsp, pad8
    pop rdi
    mov rax, err_ret
    jmp .end_pop

.valid_pop:
    push rbx
    push r12

    mov rbx, rdi
    dec ecx
    mov dword [rbx + len_offset], ecx

    mov rax, [rbx]
    movsxd rdx, ecx
    mov r12d, dword [rax + rdx * int_size]

    mov edx, dword [rbx + cap_offset]
    cmp edx, 1                  ; cap minim
    jle .skip_realloc_pop

    mov eax, edx
    shr eax, 1                  ; cap / 2
    cmp ecx, eax
    jg .skip_realloc_pop

    mov dword [rbx + cap_offset], eax
    mov rdi, [rbx]
    mov esi, eax
    shl esi, log2_int
    call realloc
    mov [rbx], rax

.skip_realloc_pop:
    movsxd rax, r12d

    pop r12
    pop rbx

.end_pop:
    leave
    ret

print_vector:
    push rbp
    mov rbp, rsp
    xor rax, rax

    push rbx
    push r12
    push r13
    sub rsp, pad8

    mov rbx, rdi

    lea rdi, [rel fmt_start]
    xor eax, eax
    call printf

    xor r12d, r12d
    mov r13d, dword [rbx + cap_offset]

.loop_print:
    cmp r12d, r13d
    jge .done_print

    cmp r12d, dword [rbx + len_offset]
    jge .print_empty_slot

    mov rax, [rbx]
    movsxd rcx, r12d
    mov esi, dword [rax + rcx * int_size]
    lea rdi, [rel fmt_elem]
    xor eax, eax
    call printf
    jmp .next_print

.print_empty_slot:
    lea rdi, [rel fmt_empty]
    xor eax, eax
    call printf

.next_print:
    inc r12d
    jmp .loop_print

.done_print:
    lea rdi, [rel fmt_end]
    mov esi, dword [rbx + len_offset]
    mov edx, dword [rbx + cap_offset]
    xor eax, eax
    call printf

    add rsp, pad8
    pop r13
    pop r12
    pop rbx

    leave
    ret

free_vector:
    push rbp
    mov rbp, rsp
    xor rax, rax

    push rbx
    sub rsp, pad8

    mov rbx, rdi
    mov rdi, [rbx]
    test rdi, rdi
    jz .done_free

    push rdi
    sub rsp, pad8

    mov rdi, [rdi]
    call free

    add rsp, pad8
    pop rdi
    call free

    mov qword [rbx], 0          ; *vec = NULL

.done_free:
    add rsp, pad8
    pop rbx

    leave
    ret
