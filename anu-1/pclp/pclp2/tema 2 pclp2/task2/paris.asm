; you can declare any helper variables in .data or .bss

section .text

;; DO NOT MODIFY
global solve_labyrinth

solve_labyrinth:
	push    rbp
	mov     rbp, rsp
	push    rbx
	push    r12
	push    r13
	push    r14
	push    r15

	mov     r12, rdi
	mov     r13, rsi
	mov     r14, rdx
	mov     r15, rcx
	mov     rbx, r8
	;; DO NOT MODIFY
	;; YOUR CODE STARTS HERE

	xor     r8, r8
	xor     r9, r9

	; initializam cu -1 (coordonata invalida) randul anterior
	mov     r10, -1
	; initializam cu -1 (coordonata invalida) coloana anterioara
	mov     r11, -1

.loop_start:
	mov     rax, r14
	dec     rax
	cmp     r8, rax
	je      .end_maze

	mov     rax, r15
	dec     rax
	cmp     r9, rax
	je      .end_maze

.try_right:
	mov     rax, r9
	inc     rax
	cmp     rax, r15
	jge     .try_down

	cmp     r8, r10
	jne     .check_right_val
	cmp     rax, r11
	je      .try_down

.check_right_val:
	; inmultim cu 8 pt ca matricea contine pointeri de 8 bytes (x86_64)
	mov     rdi, [rbx + r8 * 8]
	cmp     byte [rdi + rax], '0'
	jne     .try_down

	mov     r10, r8
	mov     r11, r9
	mov     r9, rax
	jmp     .loop_start

.try_down:
	mov     rax, r8
	inc     rax
	cmp     rax, r14
	jge     .try_left

	cmp     rax, r10
	jne     .check_down_val
	cmp     r9, r11
	je      .try_left

.check_down_val:
	; inmultim cu 8 (pointer de 64 biti) pt a accesa randul rax
	mov     rdi, [rbx + rax * 8]
	cmp     byte [rdi + r9], '0'
	jne     .try_left

	mov     r10, r8
	mov     r11, r9
	mov     r8, rax
	jmp     .loop_start

.try_left:
	; verificam daca ne aflam la marginea din stanga a matricei (coloana 0)
	cmp     r9, 0
	je      .try_up

	mov     rax, r9
	dec     rax

	cmp     r8, r10
	jne     .check_left_val
	cmp     rax, r11
	je      .try_up

.check_left_val:
	; inmultim randul curent cu 8 bytes pt a gasi adresa de inceput a vectorului
	mov     rdi, [rbx + r8 * 8]
	cmp     byte [rdi + rax], '0'
	jne     .try_up

	mov     r10, r8
	mov     r11, r9
	mov     r9, rax
	jmp     .loop_start

.try_up:
	; verificam daca ne aflam la marginea de sus a matricei (randul 0)
	cmp     r8, 0
	je      .loop_start

	mov     rax, r8
	dec     rax

	cmp     rax, r10
	jne     .check_up_val
	cmp     r9, r11
	je      .loop_start

.check_up_val:
	; inmultim indexul rax cu 8 bytes (dimensiune pointer)
	mov     rdi, [rbx + rax * 8]
	cmp     byte [rdi + r9], '0'
	jne     .loop_start

	mov     r10, r8
	mov     r11, r9
	mov     r8, rax
	jmp     .loop_start

.end_maze:
	mov     dword [r12], r8d
	mov     dword [r13], r9d

	;; YOUR CODE ENDS HERE
	;; DO NOT MODIFY
	pop     r15
	pop     r14
	pop     r13
	pop     r12
	pop     rbx
	pop     rbp
	ret
	;; DO NOT MODIFY