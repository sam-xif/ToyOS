; Keyboard driver

; Keyboard controller I/O
global _kybrd_set_leds
global _kybrd_wait_write
global _kybrd_read
global _kybrd_wait_read
global _kybrd_enable_interrupt

; Keyboard encoder I/O
global _kybrd_enc_read

section .text

; Keyboard Controller (port 0x64) IO functions
; TODO: make C functions for reading and writing to I/O ports

_kybrd_wait_write:
  push eax
  
kybrd_wait_write_loop:
  in al, 0x64
  test al, 0x2
  jnz kybrd_wait_write_loop
  
  pop eax
  ret ; return when keyboard controller is ready to be written to

_kybrd_read:
  mov eax, 0 ; clear return register
  in al, 0x64
  ret
  
_kybrd_wait_read:
  push eax
  
kybrd_wait_read_loop:
  in al, 0x64
  test al, 0x1
  jz kybrd_wait_read_loop
  
  pop eax
  ret ; return when keyboard controller is ready to be read from
  
_kybrd_set_leds:
  push ebp
  mov ebp, esp
  
  push eax
  
  call _kybrd_wait_write
  
  mov al, 0xed
  out 0x60, al
  
  mov byte eax, [ebp + 8]
  out 0x60, al ; Send the command to the keyboard encoder
  
  pop eax
  pop ebp
  ret
  
_kybrd_enable_interrupt:
  push eax
  
  call _kybrd_wait_write
  
  mov al, 0xae
  out 0x64, al
  
  call _kybrd_wait_write
  
  mov al, 0x20
  out 0x64, al
  
  call _kybrd_wait_read
  
  in al, 0x60
  mov ah, al
  
  call _kybrd_wait_write
  
  mov al, 0x60
  out 0x64, al
  
  call _kybrd_wait_write
  
  mov al, ah
  or al, 1
  or al, 4
  out 0x60, al
  
  pop eax
  ret
  
_kybrd_enc_read:
  mov eax, 0
  in al, 0x60
  ret