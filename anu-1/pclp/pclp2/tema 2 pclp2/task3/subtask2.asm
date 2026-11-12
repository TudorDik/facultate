; write the structures
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
global filter_flights

; void filter_flights(struct flight* origFlights, struct flight* finalFlights,
;                        int* nrFlights, int min_bag_weight)
; rdi = struct flight *origFlights
; rsi = struct flight *finalFlights
; rdx = int *nrFlights
; rcx = int min_bag_weight
filter_flights:
	push rbp
	mov rbp, rsp
	push rbx
	push r12
	push r13
	push r14
	push r15
	;; DO NOT MODIFY
	;; Your code starts here

	mov r8d, dword [rdx]
	test r8d, r8d
	jz .end_filter

	xor r9d, r9d

.filter_loop:
	movzx r10d, word [rdi + flight.bag_weight]
	cmp r10d, ecx
	jl .skip_flight

	mov r11, [rdi]
	mov [rsi], r11
	; copiem urmatorii 8 bytes din structura (offset 8)
	mov r11, [rdi + 8]
	; adaugam offset de 8 bytes pt array-ul destinatie
	mov [rsi + 8], r11
	; copiem urmatorii 8 bytes (offset 16)
	mov r11, [rdi + 16]
	; adaugam offset de 16 bytes pt array-ul destinatie
	mov [rsi + 16], r11
	; copiem urmatorii 8 bytes (offset 24)
	mov r11, [rdi + 24]
	; adaugam offset de 24 bytes pt array-ul destinatie
	mov [rsi + 24], r11
	; copiem urmatorii 8 bytes (offset 32)
	mov r11, [rdi + 32]
	; adaugam offset de 32 bytes pt array-ul destinatie
	mov [rsi + 32], r11
	; copiem ultimii 2 bytes ai structurii (offset 40)
	mov r11w, [rdi + 40]
	; adaugam offset de 40 bytes pt array-ul destinatie
	mov [rsi + 40], r11w

	; avansam in destinatie cu dimensiunea totala a structurii (42 bytes)
	add rsi, 42
	inc r9d

.skip_flight:
	; avansam in sursa cu dimensiunea structurii (42 bytes)
	add rdi, 42
	dec r8d
	jnz .filter_loop

.end_filter:
	mov dword [rdx], r9d

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

	leave
	ret