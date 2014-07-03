GLOBAL _int_74_hand
GLOBAL inportb, outportb
GLOBAL _setmouse

EXTERN tmp, mouse
EXTERN _mouse_handler

SECTION .text

inportb:
  push    ebp
  mov     ebp, esp
  xor dx, dx
  xor ax, ax
  mov dx, word[ebp + 8]
  in  ax, dx   ;ax: return value 
  mov esp, ebp
  pop ebp
  ret

outportb:
  push    ebp
  mov     ebp, esp
  xor dx, dx
  xor ax, ax
  mov ax, word[ebp + 12]
  mov dx, word[ebp + 8]
  out dx, ax  
  mov esp, ebp
  pop ebp
  ret

;*********************************************
_int_74_hand: ; Mouse Handler
    push    ds
    push    es                      
    pusha  

    call _mouse_handler

    mov al,20h   ; Envio de EOI generico al PIC
    out 0a0h, al ; send EOI to PIC2
    out 20h, al  ; send EOI to PIC1
    popa                            
    pop     es
    pop     ds
    iret


;*********************************************
;* This code initializes ps2 mouse.
;* source: http://f.osdev.org/viewtopic.php?f=1&t=7253
;*
_setmouse:
  mov bl,0a8h ; enable mouse cmd
  call keyboard_cmd
  call keyboard_read ; read status return

  mov bl,0d4h ; next command is for mouse
  call keyboard_cmd
  mov al,0f4h ; enable mouse device
  call keyboard_write
  call keyboard_read ; read status return
  
  ;The chunk of code below enables IRQ 12
  mov bl,0x20
  call keyboard_cmd
  call keyboard_read
  or al,2
  mov [tmp],al
  mov bl,0x60
  call keyboard_cmd
  mov al,[tmp]
  call keyboard_write 
  ret


keyboard_read:
     push ecx
     push edx
     mov ecx,0ffffh
  key_read_loop:
     in al,64h
  mov [mouse],byte 0
  test al,32
  jz short not32
  mov [mouse],byte 1
  not32: test al,1
     jnz key_read_ready
     loop key_read_loop
     mov ah,1
     jmp key_read_exit
  key_read_ready:
     push ecx
     mov ecx,32
  key_read_delay:
     loop key_read_delay
     pop ecx
     in al,60h
     xor ah,ah
  key_read_exit:
     pop edx
     pop ecx
     ret

keyboard_write:
     push ecx
     push edx
     mov dl,al
     mov ecx,0ffffh
  kbd_wrt_loop1:
     in al,64h
     test al,20h
     jz kbd_wrt_ok1
     loop kbd_wrt_loop1
     mov ah,1
     jmp kbd_wrt_exit
  kbd_wrt_ok1:
     in al,60h
     mov ecx,0ffffh
  kbd_wrt_loop:
     in al,64h
     test al,2
     jz kbd_wrt_ok
     loop kbd_wrt_loop
     mov ah,1
     jmp kbd_wrt_exit
  kbd_wrt_ok:
     mov al,dl
     out 60h,al
     mov ecx,0ffffh
  kbd_wrt_loop3:
     in al,64h
     test al,2
     jz kbd_wrt_ok3
     loop kbd_wrt_loop3
     mov ah,1
     jmp kbd_wrt_exit
  kbd_wrt_ok3:
     mov ah,8
  kbd_wrt_loop4:
     mov ecx,0ffffh
  kbd_wrt_loop5:
     in al,64h
     test al,1
     jnz kbd_wrt_ok4
     loop kbd_wrt_loop5
     dec ah
     jnz kbd_wrt_loop4
  kbd_wrt_ok4:
     xor ah,ah
  kbd_wrt_exit:
     pop edx
     pop ecx
     ret

keyboard_cmd:
     mov ecx,0ffffh
  cmd_wait:
     in al,64h
     test al,2
     jz cmd_send
     loop cmd_wait
     jmp cmd_error
  cmd_send:
     mov al,bl
     out 64h,al
     mov ecx,0ffffh
  cmd_accept:
     in al,64h
     test al,2
     jz cmd_ok
     loop cmd_accept
  cmd_error:
     mov ah,1
     jmp cmd_exit
  cmd_ok:
     xor ah,ah
  cmd_exit:
     ret
