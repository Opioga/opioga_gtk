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
     
        push cx
        push si
        add si, cx    
        mov di, 500



rev:
        std     ; set D flag.
        lodsb
        cld     ; Clear D flag.
        stosb
        loop rev
        pop si
        pop cx
        call bio
        xor ax, ax
        ret




bio:
        cld
        push cx
        push si
         
        inc si
        mov di, si
        mov si, 500
        
        rep movsb
        pop si
        pop cx
         
        inc si
        rep lodsb
        ret

string:
        db 4,"lion"

string2:
        db 5,"leway"




end: 
        nop