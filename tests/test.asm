start:
	mov sp, 0x80
	call reset
	add al, cl
	call reset
	add al, ch
	call reset
	add ax, cx
	call reset
	jmp ends
reset:
	mov ax, 1010h
	mov cx, 1010h
	mov bx, 1010h
	mov dx, 1010h
	mov si, 10
	mov di, 10
	
	mov bp, 10
	ret

ends:
	nop