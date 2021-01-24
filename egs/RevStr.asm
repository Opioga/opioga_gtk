        ;
        ; RevStr
        ; Reverse a string with di and si
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


        mov sp, 128
        push cs
        push cs
        pop ds
        pop es

        mov si, string
        call rev_str
        mov si, string2
        call rev_str
        jmp end


rev_str:
        xor cx, cx
        mov cl, [si]
        mov bx, si
        mov dx, cx
L1:
        inc si
        mov al, [si]
        push ax 
        loop L1

        mov cx, dx
        mov si, bx

L2:
        inc si
        pop ax 
        mov [si], al
        loop L2
        ret




string:
        db 4,"lion"

string2:
        db 5,"leway"




end: 
        nop