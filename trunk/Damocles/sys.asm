GLOBAL int_7F_handler
GLOBAL int_80_handler
GLOBAL int_08_handler
GLOBAL int_09_handler
GLOBAL int_74_handler
GLOBAL yield
GLOBAL mascaraPIC1
GLOBAL mascaraPIC2
GLOBAL _cli
GLOBAL _sti
GLOBAL _lidt
GLOBAL _in
GLOBAL _out
GLOBAL halt
GLOBAL _read_cr0
GLOBAL _read_cr3
GLOBAL _write_cr3
GLOBAL _write_cr0
GLOBAL syscall
GLOBAL gFlags
GLOBAL sFlags

EXTERN keyboardRoutine
EXTERN timerHandler
EXTERN mouseRoutine
EXTERN procSaveStack
EXTERN schedSchedule
EXTERN schedTicks
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

syscall:
	push ebp
	mov ebp, esp

	push ebx
	push ecx
	push edx
	push esi
	push edi

	mov eax, [ebp + 8] ; Syscall
	mov ebx, [ebp + 12]; Arg1
	mov ecx, [ebp + 16]; Arg2
	mov edx, [ebp + 20]; Arg3
	mov esi, [ebp + 24]; Arg4
	mov edi, [ebp + 28]; Arg5

	int 80h

	pop edi
	pop esi
	pop edx
	pop ecx
	pop ebx

	mov esp, ebp
	pop ebp
	ret

int_80_handler:
	push ebp
	mov ebp, esp	;StackFrame

	sti		;Los procesos pueden hacer syscalls simultaneas

	; Stack especial?

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

	; Retornal al viejo stack

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
	pushad

	push esp
	call procSaveStack

	;TODO: Obtener un stack especial

	call schedTicks	;Descuenta un tick a todos los procesos esperando

	call schedSchedule
	mov esp, eax


	mov al, 20h ;manda EOI al 2020
	out 20h, al

	popad

	iret

;yield -> Identica al anterior 08, pero no descuenta ticks
int_7F_handler:
	pushad

	push esp
	call procSaveStack

	;TODO: Obtener un stack especial

	call schedSchedule
	mov esp, eax

	popad

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


halt:
	hlt
	ret

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


yield:
	int 7Fh
	ret

gFlags:
	push ebp
	mov ebp, esp

	pushfd
	pop eax

	mov esp, ebp
	pop ebp
	ret

sFlags:
	push ebp
	mov ebp, esp

	mov eax, [ebp + 8] ; Arg1
	push eax
	popfd

	mov esp, ebp
	pop ebp
	ret


