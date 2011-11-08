
page_fault:
	xchgl %eax,(%esp)
	pushl %ecx
	pushl %edx
	push %ds
	push %es
	push %fs
	movl $0x10,%edx
	mov %dx,%ds
	mov %dx,%es
	mov %dx,%fs
	movl %cr2,%edx
	pushl %edx
	pushl %eax
	testl $1,%eax
	jne 1f     #if the last bit of the error code in eax is not zero...then the page is existing...so jump
	call do_no_page
	jmp 2f
1:	call do_wp_page
2:	addl $8,%esp    #pop the two arguments pushed in the stack befor call the function...
	pop %fs
	pop %es
	pop %ds
	popl %edx
	popl %ecx
	popl %eax
	iret
