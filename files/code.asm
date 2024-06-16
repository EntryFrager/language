push 0
pop rbx
call main
hlt

main:
		in
		pop [rbx + 0]
		push 1
		pop [rbx + 1]
		push 100000
		pop [rbx + 2]
	while_0:
		push [rbx + 2]
		push 0
		jbe while_1
		push [rbx + 0]
		push 0
		jb if_2
		push [rbx + 0]
		push 0
		jne if_1
	if_2:
		push [rbx + 1]
		out
		jmp end_if_0
	if_1:
		push [rbx + 0]
		pop [rbx + 4]
		push rbx
		push rbx
		push 4
		add
		pop rbx
		call fact
		pop rbx
		push rax
		pop [rbx + 3]
		push [rbx + 3]
		out
	end_if_0:
		push [rbx + 2]
		push 1
		sub
		pop [rbx + 2]
		jmp while_0
	while_1:
		ret

fact:
		push [rbx + 0]
		push 1
		jne if_4
		push [rbx + 0]
		pop rax
		ret
		jmp end_if_3
	if_4:
	end_if_3:
		push [rbx + 0]
		push 1
		sub
		pop [rbx + 1]
		push [rbx + 0]
		push [rbx + 1]
		pop [rbx + 2]
		push rbx
		push rbx
		push 2
		add
		pop rbx
		call fact
		pop rbx
		push rax
		mul
		pop [rbx + 0]
		push [rbx + 0]
		pop rax
		ret
hlt
