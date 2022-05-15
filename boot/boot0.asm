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
  mov al, 0x10 ; read 16 sectors 
  mov ch, 0x0
  mov cl, 0x4 ; start at fourth sector
  mov dh, 0x0
  mov ah, 0x2
  
  int 0x13
  jc error_read 
   
  ; Enable A20 gate
  mov ax, 0x2401
  int 0x15

  mov ax, 0x9000
  mov es, ax
  xor di, di
  call do_e820
  jc exit  ;; error case. TODO: Add message
  
  mov [es:0x0], ax  ; store return value (number of table entries

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
  
  ; long jump to kernel
  jmp 0x8:0x9000
  hlt
;[bits 32] 
;stage_3:
  
  
; from https://wiki.osdev.org/Detecting_Memory_(x86)#Getting_an_E820_Memory_Map
; use the INT 0x15, eax= 0xE820 BIOS function to get a memory map
; note: initially di is 0, be sure to set it to a value so that the BIOS code will not be overwritten. 
;       The consequence of overwriting the BIOS code will lead to problems like getting stuck in `int 0x15`
; inputs: es:di -> destination buffer for 24 byte entries
; outputs: bp = entry count, trashes all registers except esi
do_e820:
        mov di, 0x4          ; Set di to 0x8004. Otherwise this code will get stuck in `int 0x15` after some entries are fetched 
	xor ebx, ebx		; ebx must be 0 to start
	xor bp, bp		; keep an entry count in bp
	mov edx, 0x0534D4150	; Place "SMAP" into edx
	mov eax, 0xe820
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes
	int 0x15
	jc short .failed	; carry set on first call means "unsupported function"
	mov edx, 0x0534D4150	; Some BIOSes apparently trash this register?
	cmp eax, edx		; on success, eax must have been reset to "SMAP"
	jne short .failed
	test ebx, ebx		; ebx = 0 implies list is only 1 entry long (worthless)
	je short .failed
	jmp short .jmpin
.e820lp:
	mov eax, 0xe820		; eax, ecx get trashed on every int 0x15 call
	mov [es:di + 20], dword 1	; force a valid ACPI 3.X entry
	mov ecx, 24		; ask for 24 bytes again
	int 0x15
	jc short .e820f		; carry set means "end of list already reached"
	mov edx, 0x0534D4150	; repair potentially trashed register
.jmpin:
	jcxz .skipent		; skip any 0 length entries
	cmp cl, 20		; got a 24 byte ACPI 3.X response?
	jbe short .notext
	test byte [es:di + 20], 1	; if so: is the "ignore this data" bit clear?
	je short .skipent
.notext:
	mov ecx, [es:di + 8]	; get lower uint32_t of memory region length
	or ecx, [es:di + 12]	; "or" it with upper uint32_t to test for zero
	jz .skipent		; if length uint64_t is 0, skip entry
	inc bp			; got a good entry: ++count, move to next storage spot
	add di, 24
.skipent:
	test ebx, ebx		; if ebx resets to 0, list is complete
	jne short .e820lp
.e820f:
	;mov [mmap_ent], bp	; store the entry count
    mov ax, bp
	clc			; there is "jc" on end of list to this point, so the carry must be cleared
	ret
.failed:
	stc			; "function unsupported" error exit
	ret


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
