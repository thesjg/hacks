	.file	"goto-direct-switch.c"
	.data
	.align 4
	.type	a, @object
	.size	a, 4
a:
	.long	1
.globl ins
	.align 32
	.type	ins, @object
	.size	ins, 132
ins:
	.long	0
	.long	1
	.long	3
	.long	2
	.long	1
	.long	2
	.long	0
	.long	1
	.long	3
	.long	2
	.long	1
	.long	2
	.long	0
	.long	1
	.long	1
	.long	2
	.long	3
	.long	3
	.long	2
	.long	0
	.long	3
	.long	2
	.long	1
	.long	0
	.long	2
	.long	3
	.long	1
	.long	0
	.long	0
	.long	3
	.long	1
	.long	2
	.long	4
	.local	ic
	.comm	ic,4,4
	.section	.rodata
.LC0:
	.string	"%d\n"
	.text
.globl main
	.type	main, @function
main:
	leal	4(%esp), %ecx
	andl	$-16, %esp
	pushl	-4(%ecx)
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ecx
	subl	$52, %esp
	movl	$1, -12(%ebp)
	movl	$0, -8(%ebp)
	jmp	.L2
.L3:
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	movl	%eax, -40(%ebp)
	cmpl	$4, -40(%ebp)
	ja	.L4
	movl	-40(%ebp), %edx
	movl	.L10(,%edx,4), %eax
	jmp	*%eax
	.section	.rodata
	.align 4
	.align 4
.L10:
	.long	.L11
	.long	.L12
	.long	.L13
	.long	.L14
	.long	.L15
	.text
.L4:
	incl	-8(%ebp)
.L2:
	cmpl	$99999999, -8(%ebp)
	jle	.L3
	movl	a, %eax
	subl	$8, %esp
	pushl	%eax
	pushl	$.LC0
	call	printf
	addl	$16, %esp
	movl	$0, %eax
	movl	%eax, -44(%ebp)
	jmp	.L1
.L11:
	movl	a, %eax
	incl	%eax
	movl	%eax, a
	movl	ic, %eax
	incl	%eax
	movl	%eax, ic
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	movl	%eax, -36(%ebp)
	cmpl	$4, -36(%ebp)
	ja	.L12
	movl	-36(%ebp), %edx
	movl	.L22(,%edx,4), %eax
	jmp	*%eax
	.section	.rodata
	.align 4
	.align 4
.L22:
	.long	.L11
	.long	.L12
	.long	.L13
	.long	.L14
	.long	.L15
	.text
.L12:
	movl	a, %eax
	decl	%eax
	movl	%eax, a
	movl	ic, %eax
	incl	%eax
	movl	%eax, ic
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	movl	%eax, -32(%ebp)
	cmpl	$4, -32(%ebp)
	ja	.L13
	movl	-32(%ebp), %edx
	movl	.L28(,%edx,4), %eax
	jmp	*%eax
	.section	.rodata
	.align 4
	.align 4
.L28:
	.long	.L11
	.long	.L12
	.long	.L13
	.long	.L14
	.long	.L15
	.text
.L13:
	movl	a, %eax
	incl	%eax
	movl	%eax, a
	movl	ic, %eax
	incl	%eax
	movl	%eax, ic
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	movl	%eax, -28(%ebp)
	cmpl	$4, -28(%ebp)
	ja	.L14
	movl	-28(%ebp), %edx
	movl	.L34(,%edx,4), %eax
	jmp	*%eax
	.section	.rodata
	.align 4
	.align 4
.L34:
	.long	.L11
	.long	.L12
	.long	.L13
	.long	.L14
	.long	.L15
	.text
.L14:
	movl	a, %eax
	decl	%eax
	movl	%eax, a
	movl	ic, %eax
	incl	%eax
	movl	%eax, ic
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	movl	%eax, -24(%ebp)
	cmpl	$1, -24(%ebp)
	je	.L12
	cmpl	$1, -24(%ebp)
	jg	.L39
	cmpl	$0, -24(%ebp)
	je	.L11
	jmp	.L15
.L39:
	cmpl	$2, -24(%ebp)
	je	.L13
	cmpl	$3, -24(%ebp)
	je	.L14
.L15:
	movl	$0, ic
	jmp	.L4
.L1:
	movl	-44(%ebp), %eax
	movl	-4(%ebp), %ecx
	leave
	leal	-4(%ecx), %esp
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.1.2 (DragonFly)"
