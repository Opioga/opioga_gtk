        ;
        ; ArraySum
        ; Sums elements in array
        ;
        ; by Allison Kosy
        ; http://kosiken.github.io/
        ;
        ; (c) Copyright 2020 krc.
        ;
        ; Creation date: Dec/21/2019.
        ;
        ; Permission is hereby granted, free of charge, to any person obtaining a copy
        ; of this software and associated documentation files (the "Software"), to deal
        ; in the Software without restriction, including without limitation the rights
        ; to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
        ; copies of the Software, and to permit persons to whom the Software is
        ; furnished to do so, subject to the following conditions:

        ; The above copyright notice and this permission notice shall be included in all
        ; copies or substantial portions of the Software.

        ; THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
        ; IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
        ; FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
        ; AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
        ; LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
        ; OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
        ; SOFTWARE.    
;--------------------------------------------------------------------------------------------

;   begin

sum: equ 0x900 ; Address to store sum



        
        mov sp, 128     ; Initialize stack for use
        push cs         ; Set ds, es segments to cs = 0x7f00 
        push cs
        pop ds
        pop es
        mov ax, 5
        push ax
        mov ax, Array
    
        push ax
        call ArraySum
        call WriteDec   ; display the sum
        jmp end


ArraySum:
        push bp
        mov bp,sp	    ; set frame pointer
        push si		    ; save ESI

        mov word [sum], 0		; sum = 0
        mov si,Array	; array pointer
        mov cx,5	    ; count
    
L1:

        mov ax,[si]	    ; get array value
        add  [sum],ax	; add to sum
        
        add si,2		; next array position
        loop L1

        pop si		    ; restore si
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