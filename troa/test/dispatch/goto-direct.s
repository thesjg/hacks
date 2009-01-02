	.file	"goto-direct.c"
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
	subl	$20, %esp
	movl	$1, -12(%ebp)
	movl	$0, -8(%ebp)
	jmp	.L2
.L3:
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	testl	%eax, %eax
	je	.L4
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$1, %eax
	je	.L6
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$2, %eax
	je	.L8
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$3, %eax
	je	.L10
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$4, %eax
	je	.L12
.L13:
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
	movl	%eax, -24(%ebp)
	jmp	.L1
.L4:
	movl	a, %eax
	incl	%eax
	movl	%eax, a
	movl	ic, %eax
	incl	%eax
	movl	%eax, ic
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	testl	%eax, %eax
	je	.L4
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$1, %eax
	je	.L6
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$2, %eax
	je	.L8
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$3, %eax
	je	.L10
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$4, %eax
	je	.L12
.L6:
	movl	a, %eax
	decl	%eax
	movl	%eax, a
	movl	ic, %eax
	incl	%eax
	movl	%eax, ic
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	testl	%eax, %eax
	je	.L4
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$1, %eax
	je	.L6
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$2, %eax
	je	.L8
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$3, %eax
	je	.L10
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$4, %eax
	je	.L12
.L8:
	movl	a, %eax
	incl	%eax
	movl	%eax, a
	movl	ic, %eax
	incl	%eax
	movl	%eax, ic
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	testl	%eax, %eax
	je	.L4
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$1, %eax
	je	.L6
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$2, %eax
	je	.L8
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$3, %eax
	je	.L10
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$4, %eax
	je	.L12
.L10:
	movl	a, %eax
	decl	%eax
	movl	%eax, a
	movl	ic, %eax
	incl	%eax
	movl	%eax, ic
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	testl	%eax, %eax
	je	.L4
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$1, %eax
	je	.L6
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$2, %eax
	je	.L8
	movl	ic, %eax
	movl	ins(,%eax,4), %eax
	cmpl	$3, %eax
	je	.L10
.L12:
	movl	$0, ic
	jmp	.L13
.L1:
	movl	-24(%ebp), %eax
	movl	-4(%ebp), %ecx
	leave
	leal	-4(%ecx), %esp
	ret
	.size	main, .-main
	.ident	"GCC: (GNU) 4.1.2 (DragonFly)"
