
lee: mov bx, 7081h
	stc
	mov ax, 7081h
	mov cl, 8
	mov [bx], ax
	mov dx, [bx]
	rcl WORD [bx], cl
	mov dx, [bx]
	stc
	rcl bx, cl
	nop