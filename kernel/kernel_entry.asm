;;kernel_entry.asm

[bits 32]

global start
global _load_gdtr
global _load_idtr
global _enable_interrupts
global _generate_interrupt

extern _kmain	        ;kmain is defined in the c file

section .text
start:
  cli 			;block interrupts for now, they will be reenabled in the kernel
  call _kmain
  ;hlt		 	;halt the CPU
  
  
  
halt:
  jmp halt



; Functions that are more easily defined in assembly go here
  
_load_gdtr:
  ; create new stack frame
  push ebp
  mov ebp, esp
  
  ; reserve six bytes for the gdt register
  sub esp, 6
  
  mov ax, [ebp + 8]
  sub ax, 1 	; Subtract one because the size value that is passed to the register is 1 less than the actual size
  mov WORD [ebp - 6], ax
  
  mov eax, [ebp + 12]
  mov DWORD [ebp - 4], eax
  
  ; load the register
  lgdt [ebp - 6]
  
  ; clean up the stack
  add esp, 6
 
  
  jmp 0x8:long_jmp
  
long_jmp:
    ; Now, reset data segments and perform a long jump
  ; Setup data segments
  mov ax, 0x10		; set data segments to data selector (0x10)
  mov ds, ax
  mov ss, ax
  mov es, ax
  
  
  ; return
  pop ebp
  ret
  
_load_idtr:
    ; create new stack frame
  push ebp
  mov ebp, esp
  
  ; reserve six bytes for the gdt register
  sub esp, 6
  
  mov ax, [ebp + 8]
  mov WORD [ebp - 6], ax
  
  mov eax, [ebp + 12]
  mov DWORD [ebp - 4], eax
  
  ; load the register
  lidt [ebp - 6]
  
  ; clean up the stack
  add esp, 6
 
  ; return
  pop ebp
  ret

; Enables interrupts
_enable_interrupts:
  sti
  ret
  
; void generate_interrupt(int interrupt)
_generate_interrupt:
  push ebp
  mov ebp, esp

  mov al, byte [ebp + 8]
  mov byte [genint+1], al
  jmp genint
genint:
  int 0	; above code modifies the 0 to int number to generate
  
  pop ebp
