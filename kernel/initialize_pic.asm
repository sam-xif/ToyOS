global _initialize_pic

section .text

; http://www.brokenthorn.com/Resources/OSDevPic.html
_initialize_pic:
; Setup to initialize the primary PIC. Send ICW 1
  mov	al, 0x11
  out	0x20, al
 
; Remember that we have 2 PICs. Because we are cascading with this second PIC, send ICW 1 to second PIC command register
  out	0xA0, al	; slave PIC command register
  
  
  
  ; send ICW 2 to primary PIC
  mov	al, 0x20		; Primary PIC handled IRQ 0..7. IRQ 0 is now mapped to interrupt number 0x20
  out	0x21, al
 
	; send ICW 2 to secondary controller
  mov	al, 0x28		; Secondary PIC handles IRQ's 8..15. IRQ 8 is now mapped to use interrupt 0x28
  out	0xA1, al
  
  
  
  ; Send ICW 3 to primary PIC
  mov	al, 0x4		; 0x04 => 0100, second bit (IR line 2)
  out	0x21, al	; write to data register of primary PIC
 
	; Send ICW 3 to secondary PIC
  mov	al, 0x2		; 010=> IR line 2
  out	0xA1, al	; write to data register of secondary PIC
  
  mov	al, 1		; bit 0 enables 80x86 mode
 
	; send ICW 4 to both primary and secondary PICs
  out	0x21, al
  out	0xA1, al
  
  mov	al, 0
  out	0x21, al
  out	0xA1, al
  
  ret