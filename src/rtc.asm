GLOBAL _RTC_hand, _rtc_hand_save_prev_time, _rtc_hand_save_current_time
GLOBAL _rtc_hand_read, _rtc_hand_write

EXTERN  aux_func

EXTERN  hour
EXTERN  minute
EXTERN  seconds
EXTERN  dayofmonth
EXTERN  month
EXTERN  year

EXTERN  prev_hour
EXTERN  prev_minute
EXTERN  prev_seconds

EXTERN  current_hour
EXTERN  current_minute
EXTERN  current_seconds


section .text

;*********************************************************************
_rtc_hand_read:          ;RTC Handler
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
_rtc_hand_save_prev_time:          ;RTC Handler
    push    ds
    push    es                      
    pusha
    
    mov al, 0x00    ;seconds
    out 0x70, al
    in al, 0x71
    mov dword[prev_seconds], eax

    mov al, 0x02    ;minutes
    out 0x70, al
    in al, 0x71
    mov dword[prev_minute], eax

    mov al, 0x04    ;hours
    out 0x70, al
    in al, 0x71
    mov dword[prev_hour], eax

    ; End
    mov al,20h      ; Envio de EOI generico al PIC
    out 20h,al
    popa                            
    pop     es
    pop     ds
    ret


;*********************************************************************
_rtc_hand_save_current_time:          ;RTC Handler
    push    ds
    push    es                      
    pusha
    
    mov al, 0x00    ;seconds
    out 0x70, al
    in al, 0x71
    mov dword[current_seconds], eax

    mov al, 0x02    ;minutes
    out 0x70, al
    in al, 0x71
    mov dword[current_minute], eax

    mov al, 0x04    ;hours
    out 0x70, al
    in al, 0x71
    mov dword[current_hour], eax

    ; End
    mov al,20h      ; Envio de EOI generico al PIC
    out 20h,al
    popa                            
    pop     es
    pop     ds
    ret


;*********************************************************************
_rtc_hand_write:          ;RTC Handler
    push ebp
    mov ebp, esp

    mov ebx, dword[ebp + 8]
    xor cl, cl
    mov cl, 00000000b

    again:
    cmp ebx, 0
    jz write_
    add cl, 00000001b
    dec ebx
    jmp again


    write_:
    cli
    mov eax, dword[ebp + 12] 
    out 0x70, al
    mov al, cl
    out 0x71, al
    sti

    ; End
    mov al,20h      ; Envio de EOI generico al PIC
    out 20h,al

    mov esp, ebp
    pop     ebp                      
    ret