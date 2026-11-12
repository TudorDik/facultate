; write the structure
struc flight
	.destination: resb 32
	.dep_day: resb 1
	.dep_hour: resb 1
	.dep_min: resb 1
	.arr_day: resb 1
	.arr_hour: resb 1
	.arr_min: resb 1
	.bag_weight: resw 1
	.delay_min: resb 1
	.delay_hour: resb 1
endstruc

section .text

;; DO NOT MODIFY
global sort_and_return

; int sort_and_return(struct flight* flights, int nrFlights,
;                      struct flight* bestFlight, char destination[32])
; rdi = flights (pointer)
; rsi = nrFlights (value)
; rdx = bestFlight (pointer to pre-allocated struct)
; rcx = destination (pointer to 32-byte string)
sort_and_return:
	push rbp
	mov rbp, rsp
	push rbx
	push r12
	push r13
	push r14
	push r15
	;; DO NOT MODIFY
	;; Your code starts here

	; -- PARTEA 1: SORTAREA (Bubble Sort) --
	mov r8, rsi
	; verificam daca exista un singur element sau sir gol
	cmp r8, 1
	jle .sort_done

	xor r9, r9

.outer_loop:
	mov r10, r8
	sub r10, r9
	dec r10
	; verificam daca limita buclei interioare a ajuns la zero
	cmp r10, 0
	jle .sort_done

	xor r11, r11
	mov r12, rdi

.inner_loop:
	mov r13, r12
	; adaugam 42 bytes (dimensiunea structurii) pt al doilea zbor
	add r13, 42

	; Comparare arr_day
	mov al, byte [r12 + flight.arr_day]
	mov bl, byte [r13 + flight.arr_day]
	cmp al, bl
	ja .do_swap
	jb .no_swap

	; Comparare arr_hour
	mov al, byte [r12 + flight.arr_hour]
	mov bl, byte [r13 + flight.arr_hour]
	cmp al, bl
	ja .do_swap
	jb .no_swap

	; Comparare arr_min
	mov al, byte [r12 + flight.arr_min]
	mov bl, byte [r13 + flight.arr_min]
	cmp al, bl
	ja .do_swap
	jb .no_swap

	; Comparare bag_weight (Aici inversam: valoare mai mare = mai bine)
	mov ax, word [r12 + flight.bag_weight]
	mov bx, word [r13 + flight.bag_weight]
	cmp ax, bx
	jb .do_swap
	jmp .no_swap

.do_swap:
	push rcx
	; 42 bytes in total de interschimbat
	mov rcx, 42
.swap_loop:
	; indexare 0-based, scadem 1 pt byte din stanga
	mov al, byte [r12 + rcx - 1]
	; indexare 0-based, scadem 1 pt byte din dreapta
	mov bl, byte [r13 + rcx - 1]
	; salvam la indexul calculat (-1) valoarea din dreapta
	mov byte [r12 + rcx - 1], bl
	; salvam la indexul calculat (-1) valoarea din stanga
	mov byte [r13 + rcx - 1], al
	dec rcx
	jnz .swap_loop
	pop rcx

.no_swap:
	; avansam pointerul cu o structura de 42 bytes
	add r12, 42
	inc r11
	cmp r11, r10
	jl .inner_loop

	inc r9
	mov rax, r8
	dec rax
	cmp r9, rax
	jl .outer_loop

.sort_done:

	; -- PARTEA 2: CAUTAREA DESTINATIEI --
	xor r8, r8
	mov r9, rdi

.search_loop:
	cmp r8, rsi
	jge .not_found

	xor r10, r10

.strcmp_loop:
	; lungimea maxima a stringului este de 32 char
	cmp r10, 32
	jge .found_it

	mov al, byte [r9 + flight.destination + r10]
	mov bl, byte [rcx + r10]
	cmp al, bl
	jne .next_flight

	; comparam cu sfarsitul stringului (null terminator)
	cmp al, 0
	je .found_it

	inc r10
	jmp .strcmp_loop

.next_flight:
	; trecem la urmatorul zbor saltand 42 de bytes
	add r9, 42
	inc r8
	jmp .search_loop

.found_it:
	push rcx
	; preparam copierea celor 42 de bytes din structura
	mov rcx, 42
.copy_best:
	; copiem de la coada la cap (scazand 1 pt index)
	mov al, byte [r9 + rcx - 1]
	; introducem in structura bestFlight la fel (-1)
	mov byte [rdx + rcx - 1], al
	dec rcx
	jnz .copy_best
	pop rcx

	; returnam 1 in semn de succes
	mov rax, 1
	jmp .end_subtask3

.not_found:
	; returnam 0 daca nu am gasit zborul cautat
	mov rax, 0
	xor rdx, rdx

.end_subtask3:
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