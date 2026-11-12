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
global apply_delay

; void apply_delay(struct flight* flights, int nrFlights)
; rdi = struct flight *flights
; rsi = int nrFlights
apply_delay:
	push rbp
	mov rbp, rsp
	push rbx
	push r12
	push r13
	push r14
	push r15
	;; DO NOT MODIFY
	;; Your code starts here

	mov r9, rdi
	mov r10, rsi
	test r10, r10
	jz .end_loop

.process_flight:
	mov r11b, byte [r9 + flight.delay_min]
	mov r12b, byte [r9 + flight.delay_hour]

	movzx ax, byte [r9 + flight.dep_min]
	movzx cx, r11b
	add ax, cx
	; impartim la 60 de minute pt a afla orele in plus
	mov bl, 60
	div bl

	mov dl, ah
	mov byte [r9 + flight.dep_min], dl

	movzx cx, byte [r9 + flight.dep_hour]
	movzx dx, al
	add cx, dx
	movzx dx, r12b
	add cx, dx
	mov ax, cx
	; impartim la 24 de ore pt a afla zilele in plus
	mov bl, 24
	div bl
	mov dl, ah
	mov byte [r9 + flight.dep_hour], dl

	add byte [r9 + flight.dep_day], al

	movzx ax, byte [r9 + flight.arr_min]
	movzx cx, r11b
	add ax, cx
	; impartim la 60 de minute pt a afla orele de sosire in plus
	mov bl, 60
	div bl
	mov dl, ah
	mov byte [r9 + flight.arr_min], dl

	movzx cx, byte [r9 + flight.arr_hour]
	movzx dx, al
	add cx, dx
	movzx dx, r12b
	add cx, dx
	mov ax, cx
	; impartim la 24 de ore pt a afla zilele de sosire in plus
	mov bl, 24
	div bl
	mov dl, ah
	mov byte [r9 + flight.arr_hour], dl

	add byte [r9 + flight.arr_day], al

	; trecem la urmatorul zbor adunand 42 bytes (dimensiunea structurii)
	add r9, 42
	dec r10
	jnz .process_flight

.end_loop:
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