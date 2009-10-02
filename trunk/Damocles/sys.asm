GLOBAL _write
GLOBAL _read
GLOBAL _flush
GLOBAL new_int_80_handler
GLOBAL int_80_handler
GLOBAL int_08_handler
GLOBAL int_09_handler
GLOBAL int_74_handler
GLOBAL mascaraPIC1
GLOBAL mascaraPIC2
GLOBAL _cli
GLOBAL _sti
GLOBAL _lidt
GLOBAL _in
GLOBAL _out
GLOBAL _read_cr0
GLOBAL _read_cr3
GLOBAL _write_cr3
GLOBAL _write_cr0

EXTERN syswrite
EXTERN sysread
EXTERN sysflush
EXTERN keyboardRoutine
EXTERN timerHandler
EXTERN mouseRoutine
EXTERN _dispatcher





SECTION .text


_cli:
	cli			; limpia flag de interrupciones
	ret

_sti:

	sti			; habilita interrupciones por flag
	ret


_lidt:				; Carga el IDTR
        push    ebp
        mov     ebp, esp
        push    ebx
        mov     ebx, [ss: ebp + 6] ; ds:bx = puntero a IDTR
	rol	ebx,16
	lidt    [ds: ebx]          ; carga IDTR
        pop     ebx
        pop     ebp
        retn

_write:
	push ebp
	mov	ebp, esp
	pusha

 	mov ebx, [ebp+8]  ; file descriptor
 	mov ecx, [ebp+12] ;	buffer
 	mov edx, [ebp+16] ; cant

 	mov eax, 01h

	int 80h

	popa
	mov	esp, ebp
	pop	ebp
	ret

_read:
	push	ebp
	mov	ebp, esp
	pusha

	mov ebx, [ebp+8]  ; file descriptor
 	mov ecx, [ebp+12] ;	buffer
 	mov edx, [ebp+16] ; cant

 	mov eax, 00h

	int 80h

	popa
	mov	esp, ebp
	pop	ebp
	ret

;Realiza un flush del file descriptor indicado
;enviando la información contenida en cada buffer
;al destino correspondiente

_flush:
	push ebp
	mov ebp, esp
	pusha

	mov ebx, [ebp+8] ; file descriptor
	mov eax, 02h

	int 80h

	popa

	mov esp, ebp
	pop ebp
	ret


new_int_80_handler:
	push ebp
	mov ebp, esp	;StackFrame

	sti				;TODO cuando leer sea bloqueante
					; a nivel proceso hay que sacarlo


	; Cambio de contexto?

	push edi
	push esi
	push edx
	push ecx
	push ebx

	push esp		;Puntero al array de argumentos

	push eax		;Systemcall

	call _dispatcher
	; En eax debe dejar la
	; respuesta

	; Devolver el contexto anterior

	mov esp, ebp
	pop ebp

	iret


int_80_handler:

	push ebp
	mov ebp, esp

	sti

	push edx 	;cant
	push ecx	;buffer
	push ebx 	;fd

	cmp al, 00h ; read
	je doread
	cmp al, 01h ; write
	je dowrite
	cmp al, 02h ; flush
	je doflush

doflush:
	call sysflush
	jmp exit

doread:
	call sysread
	jmp exit

dowrite:
	call syswrite
	jmp exit

exit:


	mov esp, ebp
	pop ebp
	iret

;Retorna en ax lo que hay en el puerto pasado
;como parámetro

_in:
	push ebp
	mov ebp, esp

	push dx

	mov word dx, [ss:ebp+8]; puerto
	in al, dx

	pop dx

	mov esp, ebp
	pop ebp
	ret
_out:
	push ebp
	mov ebp, esp

	push dx
	push ax

	mov word ax, [ss:ebp+12] ; dato
	mov word dx, [ss:ebp+8]; puerto

	out dx, al

	pop ax
	pop dx


	mov esp, ebp
	pop ebp
	ret



int_08_handler:
	push ebp
	mov ebp, esp
	push eax

	cli
	call timerHandler

	mov al, 20h ;manda EOI al 2020
	out 20h, al

	pop eax
	mov esp, ebp
	pop ebp
	sti
	iret

int_09_handler:
		pusha
		in	al, 60h
		push	eax
		call	keyboardRoutine
		mov	al, 20h
		out	20h, al
		pop	eax
		popa
		iret

int_74_handler:
		pusha
		in al, 60h
		push eax
		call mouseRoutine
		mov al,20h
		out 0A0h, al ;pic slave
		out 20h, al 
		pop eax
		popa
		iret


mascaraPIC1:			; mascaraPIC1( mascara )
	push    ebp		; Escribe mascara del PIC 1
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = mascara de 16 bits
        out	21h,al		; mascara al pic master
        pop     ebp
        retn

mascaraPIC2:			; mascaraPIC2( mascara )
	push    ebp		; Escribe mascara del PIC 2
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = mascara de 16 bits
        out	0A1h,al		; mascara al pic slave
        pop     ebp
        retn



_read_cr0:
	mov eax, cr0
	retn


_write_cr0:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	mov cr0,  eax
	pop ebp
	retn


_read_cr3:
	mov eax, cr3
	retn


_write_cr3:
	push ebp
	mov ebp, esp
	mov eax, [ebp+8]
	mov cr3, eax
	pop ebp
	retn



