
	push cs
	push cs
	pop ds	
	mov cl, 3
	call init_lcd	
	lea si, [string]
	call myshow
	mov al, 0xc0
	call newline
	inc si
	call myshow
	jmp end	
commands:
	db 0x28, 0x6, 0xf

	
string:
	db "hello world", 0ah
	db "HELLO WORLD", 0ah
	
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
	cmp al, 0xa
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
		
	