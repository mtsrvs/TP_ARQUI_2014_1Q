GLOBAL  _read_msw,_lidt
GLOBAL  _int_08_hand, _int_09_hand
GLOBAL  _mascaraPIC1,_mascaraPIC2,_Cli,_Sti
GLOBAL  _debug, _rtc_hand

EXTERN  int_08
EXTERN  keyboard_handler

EXTERN  hour
EXTERN  minute
EXTERN  seconds
EXTERN  dayofmonth
EXTERN  month
EXTERN  year

SECTION .text


_Cli:
	cli			; limpia flag de interrupciones
	ret

_Sti:

	sti			; habilita interrupciones por flag
	ret

_mascaraPIC1:			; Escribe mascara del PIC 1
	push    ebp
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = mascara de 16 bits
        out	21h,al
        pop     ebp
        retn

_mascaraPIC2:			; Escribe mascara del PIC 2
	push    ebp
        mov     ebp, esp
        mov     ax, [ss:ebp+8]  ; ax = mascara de 16 bits
        out	0A1h,al
        pop     ebp
        retn

_read_msw:
        smsw    ax		; Obtiene la Machine Status Word
        retn


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


_int_08_hand:				; Handler de INT 8 ( Timer tick)
        push    ds
        push    es          ; Se salvan los registros
        pusha               ; Carga de DS y ES con el valor del selector
        mov     ax, 10h		; a utilizar.
        mov     ds, ax
        mov     es, ax                  
        call    int_08                 
        mov	al,20h			; Envio de EOI generico al PIC
	out	20h,al
	popa                            
        pop     es
        pop     ds
        iret

_int_09_hand:       ; INT 9 Handler (Keyboard)
    push    ds
    push    es                      
    pusha                           
   ; Read and process scancode
    xor ax, ax      ; Clean ax register
    in al, 60h      ; Load scancode into al register
    push ax         ; Push recently read scancode into stack
    call keyboard_handler
    ; End
    pop ax          ; Pop ascii from stack
    mov al,20h      ; Envio de EOI generico al PIC
    out 20h,al
    popa                            
    pop     es
    pop     ds
    iret

;*********************************************************************
_rtc_hand:          ;RTC Handler
    push    ds
    push    es                      
    pusha
    
    mov al, 0x00    ;seconds
    out 0x70, al
    in al, 0x71
    mov [seconds], al

    mov al, 0x02    ;minutes
    out 0x70, al
    in al, 0x71
    mov [minute], al

    mov al, 0x04    ;hours
    out 0x70, al
    in al, 0x71
    mov [hour], al

    mov al, 0x07    ;dayofmonth
    out 0x70, al
    in al, 0x71
    mov [dayofmonth], al

    mov al, 0x08    ;month
    out 0x70, al
    in al, 0x71
    mov [month], al

    mov al, 0x09    ;year
    out 0x70, al
    in al, 0x71
    mov [year], al

    ; End
    mov al,20h      ; Envio de EOI generico al PIC
    out 20h,al
    popa                            
    pop     es
    pop     ds
    ret

;*********************************************************************
_mouse_hand:          ;mouse Handler
    push    ds
    push    es                      
    pusha
    
    in ax, 0x71
    mov [seconds], al

    mov al, 0x02    ;minutes
    out 0x70, al
    in al, 0x71
    mov [minute], al

    mov al, 0x04    ;hours
    out 0x70, al
    in al, 0x71
    mov [hour], al

    mov al, 0x07    ;dayofmonth
    out 0x70, al
    in al, 0x71
    mov [dayofmonth], al

    mov al, 0x08    ;month
    out 0x70, al
    in al, 0x71
    mov [month], al

    mov al, 0x09    ;year
    out 0x70, al
    in al, 0x71
    mov [year], al

    ; End
    mov al,20h      ; Envio de EOI generico al PIC
    out 20h,al
    popa                            
    pop     es
    pop     ds
    ret
;*********************************************************************

; Debug para el BOCHS, detiene la ejecució; Para continuar colocar en el BOCHSDBG: set $eax=0
;


_debug:
        push    bp
        mov     bp, sp
        push	ax
vuelve:	mov     ax, 1
        cmp	ax, 0
	jne	vuelve
	pop	ax
	pop     bp
        retn
