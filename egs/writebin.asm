	; Version 0.02
	; Assembled by emu8086 -- must be compatible with major assemblers.
	; writebin.com -- a tiny operating system loader.
    
	; writebin [filename] [/k]    
    
	; This utility is be used from command prompt,
	; it reads a specified file and writes it to floppy drive at
	; cylinder: 0, head: 0, sector: 1  (boot sector).

	; If there is /k parameter after the file name, then the
	; file is written at cylinder: 0, head: 0, sector: 2.
	; The first sector of a floppy drive is reserved for a boot sector.
	; The second and other sectors can be used to store any kind of data
	; and code. The kernel module of tiny operating system should be
	; written to sector 2.

	; This program works with drive A: only.
	; For the emulator physical drive A: is this file c:\emu8086\FLOPPY_0
	; (for BIOS interrupts: INT 13h and boot).
	; Note: for DOS interrupts (INT 21h) drive A: is emulated in this subdirectory:
	; c:\emu8086\vdrive\a\

	; filename must be in 8.3 format, 
	; maximum 8 chars for the name and 3 chars after the dot.

	; warning! if you run this program outside of the emulator 
	;           it may corrupt existing data. use with caution.
	; do not play with floppies that contain valuable data.
main: jmp st	
	inc si
	inc di
	jmp end

filename:    db 128 dup(0)       ; full path up to 128 chars can be specified.
buffer:      db 512 dup (0)
	;buffer_size: =  $ - offset buffer
handle:      dw 0
kernel_flag: db 0               ; if there is /k parameter, kernel_flag=1.

counter: dw 0



sect:  db 1 ; sector number (1..18).
cyld:  db 0 ; cylinder number (0..79).
head:  db 0 ; head number (0..1).
drive: db 0 ; drive number (0..3) ; A:=0, B:=1...

	; init
st: mov ax, cs
	mov ds, ax
	mov es, ax
	call clear_screen


	; read cmd parameters
cp: xor cx, cx
	mov cl, [80h]
	;jcxz np                    ; this instruction is assembled into OR CX, CX and two JMPs, tutorial 7 tells why.
	mov si, 82h
	mov di, offset filename
	cld
	rep movsb 
	mov al, 0
	push di
	dec di
	mov [di], al
	pop di



clear_screen:
	push    ax      ; store registers...
	push    ds      ;
	push    bx      ;
	push    cx      ;
	push    di      ;

	mov     ax, 40h
	mov     ds, ax  ; for getting screen parameters.
	mov     ah, 06h ; scroll up function id.
	mov     al, 0   ; scroll all lines!
	mov     bh, 07  ; attribute for new lines.
	mov     ch, 0   ; upper row.
	mov     cl, 0   ; upper col.
	mov     di, 84h ; rows on screen -1,
	mov     dh, [di] ; lower row (byte).
	mov     di, 4ah ; columns on screen,
	mov     dl, [di]
	dec     dl      ; lower col.
	int     10h

	; set cursor position to top
	; of the screen:
	mov     bh, 0   ; current page.
	mov     dl, 0   ; col.
	mov     dh, 0   ; row.
	mov     ah, 02
	int     10h

	pop     di      ; re-store registers...
	pop     cx      ;
	pop     bx      ;
	pop     ds      ;
	pop     ax      ;

	ret


end: nop
	