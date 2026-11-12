section .text

;; DO NOT MODIFY
global fix_lap_times

fix_lap_times:
	push rbp
	mov rbp, rsp
	push rbx
	push r12
	push r13
	push r14
	push r15
	;; DO NOT MODIFY
	;; YOUR CODE STARTS HERE

	xor r9, r9
	xor r10, r10

.loop_start:
	cmp r9, rdx
	jge .loop_end

	mov al, byte [rsi + r9]
	; verificam daca valoarea din senzor indica o eroare (1)
	cmp al, 1
	je .has_error

.no_error:
	; inmultim indexul cu 4 pt ca elementele sunt de tip dword (4 bytes)
	mov eax, dword [rdi + r9 * 4]
	; folosim acelasi offset de 4 bytes pt array-ul destinatie
	mov dword [rcx + r9 * 4], eax
	jmp .next_iteration

.has_error:
	inc r10

	; verificam daca ne aflam la inceputul array-ului (index 0)
	cmp r9, 0
	je .first_element

	mov r11, rdx
	dec r11
	cmp r9, r11
	je .last_element

.middle_element:
	; offset de 4 bytes per element, scadem 4 pt a accesa elementul anterior
	mov eax, dword [rdi + r9 * 4 - 4]
	; offset de 4 bytes, adunam 4 pt a accesa elementul urmator
	add eax, dword [rdi + r9 * 4 + 4]
	; shiftam 1 bit la dreapta pt a imparti suma la 2
	shr eax, 1
	; offset de 4 bytes pt a salva rezultatul
	mov dword [rcx + r9 * 4], eax
	jmp .next_iteration

.first_element:
	; accesam elementul urmator (+4 bytes) pt a inlocui eroarea
	mov eax, dword [rdi + r9 * 4 + 4]
	; offset de 4 bytes pt array-ul destinatie
	mov dword [rcx + r9 * 4], eax
	jmp .next_iteration

.last_element:
	; accesam elementul anterior (-4 bytes) pt a inlocui eroarea
	mov eax, dword [rdi + r9 * 4 - 4]
	; offset de 4 bytes pt array-ul destinatie
	mov dword [rcx + r9 * 4], eax

.next_iteration:
	inc r9
	jmp .loop_start

.loop_end:
	mov dword [r8], r10d

	;; YOUR CODE ENDS HERE
	;; DO NOT MODIFY
	pop r15
	pop r14
	pop r13
	pop r12
	pop rbx
	pop rbp
	ret
	;; DO NOT MODIFY