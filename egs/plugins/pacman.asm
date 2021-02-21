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
	
main:	
	push cs
	push cs
	pop ds	
	mov ax,0xff00
	mov ss,ax
	
	mov cl, 4
	cmp cl,4
	inc di
	call init_lcd	
	mov di, 0
	
	
	
	call load_pacman
	mov sp, 0x80
	mov bl, 0
	jmp run_anim

	


newline:
	mov ah, al
	
	mov cl, 4
	shr al, cl
	or al, 30h
	out 0xff, al
	and al, 2fh
	out 0xff, al
	mov al, ah
	and al, 0xf
	or al, 30h
	out 0xff, al
	and al, 2fh
	out 0xff, al
	ret
	
	
init_lcd:
	lea si, [commands]
write:	mov al, [si]
	mov ah, al
	push cx
	mov cl, 4
	shr al, cl
	or al, 30h
	out 0xff, al
	and al, 2fh
	out 0xff, al
	mov al, ah
	and al, 0xf
	or al, 30h
	out 0xff, al
	and al, 2fh
	out 0xff, al
	pop cx
	inc si
	loop write
	ret
myshow:
	mov al, [si]
	cmp al, 33
	jz end_myshow
	mov ch, al
	mov cl,4
	
	shr al, cl
	or al, 10h
	out 0xff, al
	and al, 0xf
	out 0xff, al
	mov al,ch
	and al, 0xf
	or al, 10h
	out 0xff, al
	and al, 0xf
	out 0xff, al	
	inc si
	jmp myshow

end_myshow:
	ret	


load_pacman:
    lea si, [pacman]
	call myshow
	ret
	
clear_lcd:
    xor ax, ax
    call newline
	ret
    
	


run_anim:
	mov sp, 0x80
	mov bp, 0x80
	mov ax, bp
	add ax, di
	call newline
	mov al, dl
	inc dl
	and al, 1
	call write_lcd
	mov ax, bp
	add ax, di
	call newline
	call delay
	mov al, ' '
	call write_lcd
	
	inc di

	cmp di, 17
	jz end
	jmp run_anim

write_lcd:
	mov ch, al
	mov cl,4
	
	shr al, cl
	or al, 10h
	out 0xff, al
	and al, 0xf
	out 0xff, al
	mov al,ch
	and al, 0xf
	or al, 10h
	out 0xff, al
	and al, 0xf
	out 0xff, al	
	ret
	
delay: mov cl, 0x10
cont:	dec cl
	cmp cl, 0
	jz ends
	jmp cont
ends:
	ret	
	


commands:
	db 0x28,0x6,0xd,0x40

	
pacman:
	db  0x00,0x00,0x0E,0x1B,0x1C,0x0E,0x00,0x00,0x00,0x00,0x0E,0x1B,0x1F,0x0E,0x00,0x00,33
	



end: 
	nop;