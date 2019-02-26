;
; TODO: Fix reading so that it retries adequately and waits for the drive to be ready,
;			because Bochs is not representative of real hardware.
;		Also, implement multiple methods for enabling A20 line.
;
;


[bits 16]
[org 0x7c00]

;
; Sector 1
; Offset 0x7c00
;


  cli
  cld
  jmp 0x0:start
  
start:
; setup stack
  xor ax, ax             ;Real mode flat memory model
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x9000
  
  mov si, hello_str
  call print_str

  mov si, reset_drive_str
  call print_str
  
; reset drive
  xor ah, ah
  int 13h
  jc error_reset
  
  mov si, completed_without_err
  call print_str
  
  mov si, loading_stage2
  call print_str

; initialise essential segment registers to prepare to load the second sector
  mov bx, 0x7e00
  mov al, 0x2 ; read 2 sectors
  mov ch, 0x0
  mov cl, 0x2 ; start at second sector
  mov dh, 0x0
  mov ah, 0x2
  
  ; Load stage 2 into 0x7e00 and gdt info into 0x8000
  int 0x13
  jc error_read
  
  mov si, completed_without_err
  call print_str
  
  mov si, control_transfer
  call print_str
  
  jmp 0x0:0x7e00
  
  
error_reset:
  mov si, error_reset_str
  call print_str
  jmp exit
  
error_read:
  mov si, error_read_str
  call print_str
  jmp exit
  
exit:
  hlt
  
  
print_str:
  ; si contains pointer to null_terminated string
  push ax
  mov ah, 0x0e ; teletype output
print_str_next_char:
  lodsb
  or al, al
  jz print_str_done
  int 0x10
  jmp print_str_next_char
print_str_done:
  pop ax
  ret

; Error strings
error_reset_str db '[ERROR] Error while resetting drive.',0x0d,0x0a,0
error_read_str db '[ERROR] Error while reading sectors.',0x0d,0x0a,0

; Progress notifications
hello_str db '[STATUS] Hello, world!',0x0d,0x0a,0
reset_drive_str db '[STATUS] Resetting drive...',0x0d,0x0a,0
loading_stage2 db '[STATUS] Loading stage 2 bootloader into 0x7e00...',0x0d,0x0a,0
loading_gdt db '[STATUS] Loading GDT into 0x0500...',0x0d,0x0a,0
setting_gdtr db '[STATUS] Loading GDTR...',0x0d,0x0a,0
completed_without_err db '[SUCCESS] Operation completed without error.',0x0d,0x0a,0
control_transfer db '[STATUS] Transferring control to stage 2...',0x0d,0x0a,0
	

times 510 - ($ - $$) db 0
dw 0xAA55
;
; Sector 2
; Offset 0x7e00
;
  
stage_2:
  mov si, info_str
  call print_str
  
  mov si, loading_gdt
  call print_str
  
  mov si, setting_gdtr
  call print_str
  
  lgdt [gdtr]
  
  mov si, load_idtr
  call print_str
  
  ;lidt [idtr]
  
  ; Load kernel
  mov ax, 0x0900
  mov es, ax 
  xor ax, ax
  mov bx, ax ; load kernel to address 0xF001:0xFFF0 in memory
  mov al, 0x6 ; read 6 sectors 
  mov ch, 0x0
  mov cl, 0x4 ; start at fourth sector
  mov dh, 0x0
  mov ah, 0x2
  
  int 0x13
  jc error_read
  
   
  ; Enable A20 gate
  mov ax, 0x2401
  int 0x15
  
  mov si, protected_mode
  call print_str
  cli
  
  ; Turn on protected mode
  mov eax, cr0
  or eax, 1
  mov cr0, eax
 
   ; Setup data segments
  mov ax, 0x10		; set data segments to data selector (0x10)
  mov ds, ax
  mov ss, ax
  mov es, ax
  xor ebp, ebp
  mov esp, 90000h; stack begins from 0x90000, and goes up to 0x9ffff
 
  ; Clear prefetch queue
  jmp clear_prefetch_queue
  nop
  nop
clear_prefetch_queue:

  ; Set code segment
  ;jmp 0x8:stage_3
  
  ; long jump to 1 megabyte
  jmp 0x8:0x9000
  hlt
;[bits 32] 
;stage_3:
  
  
 ; hlt

[bits 16]

info_str db '[STATUS] Stage 2',0x0d,0x0a,0
load_idtr db '[STATUS] Loading IDTR...',0x0d,0x0a,0
protected_mode db '[STATUS] Entering protected mode...',0x0d,0x0a,0
  
idtr:
	dw 0x800	; size of IDT (0x800)
	dd 0x0		; base of IDT

gdtr: 
	dw 0x18 - 1 	; limit (Size of GDT) (0x400)
	dd 0x8000
times 512 - ($ - stage_2) db 0

[bits 16]

;
; Sector 3
; Contains the GDT. It is loaded at 0x0500
;

gdt:
  ; This is the beginning of the GDT. Because of this, its offset is 0.
 
; null descriptor 
	dd 0 				; null descriptor--just fill 8 bytes with zero
	dd 0 
 
; Notice that each descriptor is exactally 8 bytes in size. THIS IS IMPORTANT.
; Because of this, the code descriptor has offset 0x8.
 
; code descriptor:			; code descriptor. Right after null descriptor
	dw 0FFFFh 			; limit low
	dw 0 				; base low
	db 0 				; base middle
	db 10011010b 			; access
	db 11001111b 			; granularity
	db 0 				; base high
 
; Because each descriptor is 8 bytes in size, the Data descritpor is at offset 0x10 from
; the beginning of the GDT, or 16 (decimal) bytes from start.
 
; data descriptor:			; data descriptor
	dw 0FFFFh 			; limit low (Same as code)
	dw 0 				; base low
	db 0 				; base middle
	db 10010010b 			; access
	db 11001111b 			; granularity
	db 0				; base high
	
times 512 - ($ - gdt) db 0	
