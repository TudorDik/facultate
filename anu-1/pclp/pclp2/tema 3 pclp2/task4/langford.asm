section .note.GNU-stack

extern malloc
extern realloc
extern free

section .bss
g_seq:        resq 1
g_used:       resq 1
g_results:    resq 1
g_n:          resd 1
g_count:      resd 1
g_capacity:   resd 1

section .text

global check_langford
global generate_langford_sequences

check_langford:
    push rbp
    mov rbp, rsp

    mov eax, esi
    and eax, 1                  ; paritate
    jnz .fail

    mov r8d, esi
    shr r8d, 1                  ; n = len/2
    mov r9d, 1                  ; k = 1

.loop_k:
    cmp r9d, r8d
    jg .success

    xor r10d, r10d
.find_k:
    cmp r10d, esi
    jge .fail
    mov eax, dword [rdi + r10*4]    ; offset 4
    cmp eax, r9d
    je .found_k
    inc r10d
    jmp .find_k

.found_k:
    mov r11d, r10d
    add r11d, r9d
    inc r11d                    ; idx + k + 1

    cmp r11d, esi
    jge .fail

    mov eax, dword [rdi + r11*4]    ; offset 4
    cmp eax, r9d
    jne .fail

    inc r9d
    jmp .loop_k

.success:
    mov eax, 1                  ; true
    jmp .done

.fail:
    xor eax, eax

.done:
    leave
    ret


generate_langford_sequences:
    push rbp
    mov rbp, rsp
    push rbx
    sub rsp, 8                  ; aliniere

    mov [rel g_n], edi
    mov rbx, rsi

    mov qword [rel g_results], 0    ; init
    mov dword [rel g_count], 0      ; init
    mov dword [rel g_capacity], 0   ; init

    mov eax, edi
    shl eax, 3                  ; 8n bytes
    mov edi, eax
    call malloc
    mov [rel g_seq], rax

    mov rdi, rax
    mov ecx, [rel g_n]
    shl ecx, 1                  ; 2n elem
    xor eax, eax
    cld
    rep stosd

    mov edi, [rel g_n]
    inc edi
    call malloc
    mov [rel g_used], rax

    mov rdi, rax
    mov ecx, [rel g_n]
    inc ecx
    xor eax, eax
    rep stosb

    call backtrack_langford

    mov ecx, [rel g_count]
    mov [rbx], ecx

    mov rdi, [rel g_seq]
    call free
    mov rdi, [rel g_used]
    call free

    mov rax, [rel g_results]

    add rsp, 8                  ; restore
    pop rbx
    leave
    ret


backtrack_langford:
    push rbp
    mov rbp, rsp
    push rbx
    push r12
    push r13
    push r14
    push r15
    sub rsp, 8                  ; aliniere

    mov rbx, [rel g_seq]
    mov r12d, [rel g_n]
    mov r13d, r12d
    shl r13d, 1                 ; 2n

    xor ecx, ecx
.find_empty:
    cmp ecx, r13d
    jge .all_filled
    cmp dword [rbx + rcx*4], 0  ; offset 4
    je .empty_found
    inc ecx
    jmp .find_empty

.all_filled:
    call save_sequence
    jmp .leave_bt

.empty_found:
    mov r14d, ecx
    mov r15d, 1                 ; v = 1

.try_loop:
    cmp r15d, r12d
    jg .leave_bt

    mov rax, [rel g_used]
    cmp byte [rax + r15], 0     ; folosit?
    jne .skip_v

    mov edx, r14d
    add edx, r15d
    inc edx                     ; p+v+1

    cmp edx, r13d
    jge .skip_v

    cmp dword [rbx + rdx*4], 0  ; liber?
    jne .skip_v

    mov dword [rbx + r14*4], r15d   ; pune v
    mov dword [rbx + rdx*4], r15d   ; si la perechea
    mov rax, [rel g_used]
    mov byte [rax + r15], 1     ; marcheaza folosit

    call backtrack_langford

    mov edx, r14d
    add edx, r15d
    inc edx

    mov dword [rbx + r14*4], 0  ; sterge
    mov dword [rbx + rdx*4], 0  ; idem
    mov rax, [rel g_used]
    mov byte [rax + r15], 0     ; reseteaza

.skip_v:
    inc r15d
    jmp .try_loop

.leave_bt:
    add rsp, 8                  ; restore
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    leave
    ret


save_sequence:
    push rbp
    mov rbp, rsp
    push rbx
    sub rsp, 8                  ; aliniere

    mov eax, [rel g_count]
    cmp eax, [rel g_capacity]
    jl .has_room

    mov edx, [rel g_capacity]
    test edx, edx
    jnz .double_cap
    mov edx, 4                  ; cap initial
    jmp .set_cap
.double_cap:
    shl edx, 1                  ; *= 2
.set_cap:
    mov [rel g_capacity], edx

    mov rdi, [rel g_results]
    movsxd rsi, edx
    shl rsi, 3                  ; *8 (sizeof ptr)
    call realloc
    mov [rel g_results], rax

.has_room:
    mov edi, [rel g_n]
    shl edi, 3                  ; 8n bytes
    call malloc
    mov rbx, rax

    mov rdi, rbx
    mov rsi, [rel g_seq]
    mov ecx, [rel g_n]
    shl ecx, 1                  ; 2n
    cld
    rep movsd

    mov rax, [rel g_results]
    mov ecx, [rel g_count]
    mov [rax + rcx*8], rbx      ; offset 8 ptr
    inc ecx
    mov [rel g_count], ecx

    add rsp, 8                  ; restore
    pop rbx
    leave
    ret
