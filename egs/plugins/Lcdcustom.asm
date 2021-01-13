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

	push cs
	push cs
	pop ds	
	mov cl, 4
	call init_lcd	
	lea si, [string]
	call myshow
	
	mov al, 0x80
	call newline

	inc si
	call myshow
	jmp end	
commands:
	db 0x28, 0x6, 0xf, 0x40

	
string:
	db 0,8,12,14,15,14,12,8,0,2,6,14,30,14,6,2, 32
	db 0, 1, 32
	
newline:
	mov ah, al
	
	mov cl, 4
	shr al, cl
	or al, 30h
	out 199, al
	and al, 2fh
	out 199, al
	mov al, ah
	and al, 0xf
	or al, 30h
	out 199, al
	and al, 2fh
	out 199, al
	ret
	
	
init_lcd:
	lea si, [commands]
write:	mov al, [si]
	mov ah, al
	push cx
	mov cl, 4
	shr al, cl
	or al, 30h
	out 199, al
	and al, 2fh
	out 199, al
	mov al, ah
	and al, 0xf
	or al, 30h
	out 199, al
	and al, 2fh
	out 199, al
	pop cx
	inc si
	loop write
	ret
myshow:
	mov al, [si]
	cmp al, 32
	jz end_myshow
	mov ch, al
	mov cl,4
	
	shr al, cl
	or al, 10h
	out 199, al
	and al, 0xf
	out 199, al
	mov al,ch
	and al, 0xf
	or al, 10h
	out 199, al
	and al, 0xf
	out 199, al	
	inc si
	jmp myshow

end_myshow:
	ret	

delay: mov dl, 0xa
cont:	dec dl
	cmp dl, 0
	jz end_myshow
	jmp cont
	
end: 
	nop
		
	