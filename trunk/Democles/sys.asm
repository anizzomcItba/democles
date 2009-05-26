GLOBAL _write
GLOBAL _read
GLOBAL int_80_handler
GLOBAL int_08_handler
GLOBAL mascaraPIC1
GLOBAL mascaraPIC2
GLOBAL _cli
GLOBAL _sti
GLOBAL _lidt

EXTERN syswrite
EXTERN sysread




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
	push	ebp
	mov	ebp, esp


	mov	esp, ebp
	pop	ebp
	ret


_read:
	push	ebp
	mov	ebp, esp

	mov	esp, ebp
	pop	ebp
	ret

int_80_handler:
	ret

int_08_handler:

	ret

mascaraPIC1:			; mascaraPIC1( mascara )
	push    ebp		; Escribe mascara del PIC 1
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = mascara de 16 bits
        out	21h,al
        pop     ebp
        retn

mascaraPIC2:			; mascaraPIC2( mascara )
	push    ebp		; Escribe mascara del PIC 2
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = mascara de 16 bits
        out	0A1h,al
        pop     ebp
        retn

