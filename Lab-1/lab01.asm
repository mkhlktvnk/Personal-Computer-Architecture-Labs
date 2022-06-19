.model small
.stack 100h
.data
write_error db "Write error!",0Dh,0Ah,'$'
read_error db "Read error!",0Dh,0Ah,'$'
info db "Byte: ", '$'
.code
jmp start

init_com_port_1 proc
	xor ax, ax
	mov al, 10100011b
	mov dx, 0
	int 14h
ret
init_com_port_1 endp

send_byte proc
	mov al, 'A'
	mov ah, 1
	mov dx, 0
	int 14h
	test al, 80h
	jnz handle_write_error
	ret
send_byte endp

handle_write_error proc
	mov ah, 9
	mov dx, offset write_error
	add dx, 2
	int 21h
	ret
handle_write_error endp

receive_byte proc
	mov ah, 2
	mov dx, 1
	int 14h
	test al, 80h
	jnz handle_read_error
	ret
receive_byte endp

handle_read_error proc
	mov ah, 9
	mov dx, offset read_error
	add dx, 2
	int 21h
	ret
handle_read_error endp

print_received_byte proc
	mov ah, 02h
	mov dl, al
	int 21h
	ret
print_received_byte endp

quit proc
	mov ax,4C00h
	int 21h
	ret
quit endp

start:
	mov ax, @data
	mov ds, ax
	call init_com_port_1
	call send_byte
	mov al, 'a'
	call receive_byte
	mov ah, 02h
	mov dl, al
	int 21h
	call quit
end start
