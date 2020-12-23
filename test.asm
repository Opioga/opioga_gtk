        ;
        ; Pillman
        ;
        ; by Oscar Toledo G.
        ; http://nanochess.org/
        ;
        ; (c) Copyright 2019 Oscar Toledo G.
        ;
        ; Creation date: Jun/11/2019.
        ; Revision date: Jun/12/2019. Draws level.
        ; Revision date: Jun/13/2019. Pillman can move.
        ; Revision date: Jun/14/2019. Now ghosts don't get stuck. Ghost are
        ;                             transparent. Pillman doesn't leave
        ;                             trash.
        ; Revision date: Jun/15/2019. Ghosts can catch pillman. Optimized.
        ;                             509 bytes.
        ; Revision date: Jul/09/2019. Self-modifying code, move subroutine,
        ;                             cache routine address (Peter Ferrie).
        ;                             504 bytes.
        ; Revision date: Jul/22/2019. Added Esc key to exit.
        ;


sum: equ 0x900
        ; Rev str
        mov sp, 128
        push cs
        push cs
        pop ds
        pop es
        mov ax, 5
	push ax
	mov ax, Array
 
        push ax
	call ArraySum
        mov bx,5
	call WriteDec   ; display the sum
        jmp end


ArraySum:
	push bp
	mov bp,sp	; set frame pointer
        sub sp,2		; create the sum variable
	push si		; save ESI

	mov word [sum], 0		; sum = 0
	mov si,Array	; array pointer
	mov cx,5	; count
    
L1:

        mov ax,[si]	; get array value
	add  [sum],ax	; add to sum
	
	add si,2		; next array position
        loop L1

	pop si		; restore ESI
	pop bp
	ret		






        WriteDec:
        mov ax, [sum]
        int 40
        ret


Array:
        dw 50
        dw 50
        dw 50
        dw 50
        dw 50



end: 
        nop