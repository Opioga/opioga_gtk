; counts the number of characters of a zero terminated string.
push cs
	push cs
	pop ds
	pop es
mov bx, 0
jmp start
 str1: db "abcdefg hijklmnop qrstvuwxyz", 0



start: mov cx, ffh 
	
	mov bx, str1 ; load address of string.

       mov al, 0 ; reset counter.

compare: 	mov cl, [bx]
	cmp cl, 0 ; is it end of string?
         je done  ; if zero, then it's the end.

         inc ax   ; count char.
         inc bx   ; next memory position in string.
         jmp compare

; print result in binary:
done:
mov bx, ax
mov cx, 8
print: mov ah, 2   ; print function.
       mov dl, '0'
       test bl, 10000000b  ; test first bit.
       jz zero
       mov dl, '1'
zero:  int 21h
       shl bl, 1
loop print

; print binary suffix:
mov dl, 'b'
int 21h


; wait for any key press....
mov ah, 0
int 16h 


 