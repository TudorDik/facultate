section .text

;; DO NOT MODIFY
global check_column
global check_row
global check_box

; int check_row(int **array, int size, int rowNr)
; rdi = int **array
; rsi = int size
; rdx = int rowNr
check_row:
	push rbp
	mov rbp, rsp
	push rbx
	push r12
	push r13
	push r14
	push r15
	;; DO NOT MODIFY
	;; Your code starts here

	xor     r8, r8
	; initializam produsul cautat cu 1
	mov     r9, 1
	; incepem numaratoarea (contorul) de la 1
	mov     rcx, 1
.calc_targets:
	cmp     rcx, rsi
	jg      .targets_done
	add     r8, rcx
	imul    r9, rcx
	inc     rcx
	jmp     .calc_targets
.targets_done:

	; inmultim cu 8 pt ca avem array de pointeri (x86_64)
	mov     rbx, [rdi + rdx * 8]
	xor     r11, r11
	; initializam produsul elementelor de pe rand cu 1
	mov     r12, 1
	xor     rcx, rcx

.row_loop:
	cmp     rcx, rsi
	jge     .row_check
	; offset de 4 bytes pt ca elementele sunt int (32 biti)
	movsxd  rax, dword [rbx + rcx * 4]
	add     r11, rax
	imul    r12, rax
	inc     rcx
	jmp     .row_loop

.row_check:
	xor     rax, rax
	cmp     r11, r8
	jne     .done
	cmp     r12, r9
	jne     .done
	; returnam 1 in caz de validare cu succes
	mov     rax, 1
.done:
	;; Your code ends here
	;; DO NOT MODIFY
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	pop rbp
	ret
	;; DO NOT MODIFY

;; DO NOT MODIFY
; int check_column(int **array, int size, int columnNr)
; rdi = int **array
; rsi = int size
; rdx = int columnNr
check_column:
	push rbp
	mov rbp, rsp
	push rbx
	push r12
	push r13
	push r14
	push r15
	;; DO NOT MODIFY
	;; Your code starts here

	xor     r8, r8
	; initializam produsul tinta cu 1
	mov     r9, 1
	; pornim indexul de la 1
	mov     rcx, 1
.calc_targets:
	cmp     rcx, rsi
	jg      .targets_done
	add     r8, rcx
	imul    r9, rcx
	inc     rcx
	jmp     .calc_targets
.targets_done:

	xor     r11, r11
	; initializam produsul curent al coloanei cu 1
	mov     r12, 1
	xor     rcx, rcx

.col_loop:
	cmp     rcx, rsi
	jge     .col_check
	; aducem pointerul randului, dimensiune 8 bytes
	mov     rbx, [rdi + rcx * 8]
	; preluam elementul int, dimensiune 4 bytes
	movsxd  rax, dword [rbx + rdx * 4]
	add     r11, rax
	imul    r12, rax
	inc     rcx
	jmp     .col_loop

.col_check:
	xor     rax, rax
	cmp     r11, r8
	jne     .done
	cmp     r12, r9
	jne     .done
	; marcam coloana ca valida returnand 1
	mov     rax, 1
.done:
	;; Your code ends here
	;; DO NOT MODIFY
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	pop rbp
	ret
	;; DO NOT MODIFY

;; DO NOT MODIFY
; int check_box(int **array, int size, int boxNr)
; rdi = int **array
; rsi = int size
; rdx = int boxNr
check_box:
	push rbp
	mov rbp, rsp
	push rbx
	push r12
	push r13
	push r14
	push r15
	;; DO NOT MODIFY
	;; Your code starts here

	; incepem cautarea radacinii de la 1
	mov     r14, 1
.sqrt_loop:
	mov     rax, r14
	imul    rax, r14
	cmp     rax, rsi
	je      .sqrt_found
	inc     r14
	jmp     .sqrt_loop
.sqrt_found:

	mov     rax, rdx
	xor     rdx, rdx
	div     r14
	mov     r15, rax
	imul    r15, r14
	mov     r13, rdx
	imul    r13, r14

	xor     r8, r8
	; initializam produsul tinta al cutiei cu 1
	mov     r9, 1
	; pornim calculul sumei/produsului asteptat de la 1
	mov     rcx, 1
.calc_targets:
	cmp     rcx, rsi
	jg      .targets_done
	add     r8, rcx
	imul    r9, rcx
	inc     rcx
	jmp     .calc_targets
.targets_done:

	xor     r11, r11
	; produsul real al cutiei pleaca de la 1
	mov     r12, 1
	xor     r10, r10

.box_outer:
	cmp     r10, r14
	jge     .box_check
	xor     rcx, rcx

.box_inner:
	cmp     rcx, r14
	jge     .box_outer_next

	mov     rax, r15
	add     rax, r10
	; offset 8 bytes pt pointer la rand
	mov     rbx, [rdi + rax * 8]

	mov     rax, r13
	add     rax, rcx
	; offset 4 bytes pt valoare de tip int
	movsxd  rax, dword [rbx + rax * 4]

	add     r11, rax
	imul    r12, rax

	inc     rcx
	jmp     .box_inner

.box_outer_next:
	inc     r10
	jmp     .box_outer

.box_check:
	xor     rax, rax
	cmp     r11, r8
	jne     .done
	cmp     r12, r9
	jne     .done
	; totul e in regula, returnam 1
	mov     rax, 1
.done:
	;; Your code ends here
	;; DO NOT MODIFY
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	pop rbp
	ret