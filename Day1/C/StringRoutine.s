	.file	"StringRoutine.c"
	.text
	.globl	reverse
	.def	reverse;	.scl	2;	.type	32;	.endef
	.seh_proc	reverse
reverse:
	pushq	%rbp
	.seh_pushreg	%rbp
	movq	%rsp, %rbp
	.seh_setframe	%rbp, 0
	subq	$16, %rsp
	.seh_stackalloc	16
	.seh_endprologue
	movq	%rcx, 16(%rbp)
	movl	%edx, 24(%rbp)
	movl	$0, -4(%rbp)
	jmp	.L2
.L3:
	movl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movzbl	(%rax), %eax
	movb	%al, -5(%rbp)
	movl	24(%rbp), %eax
	subl	$1, %eax
	subl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	16(%rbp), %rax
	addq	%rdx, %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rcx
	movq	16(%rbp), %rdx
	addq	%rcx, %rdx
	movzbl	(%rax), %eax
	movb	%al, (%rdx)
	movl	24(%rbp), %eax
	subl	$1, %eax
	subl	-4(%rbp), %eax
	movslq	%eax, %rdx
	movq	16(%rbp), %rax
	addq	%rax, %rdx
	movzbl	-5(%rbp), %eax
	movb	%al, (%rdx)
	addl	$1, -4(%rbp)
.L2:
	movl	24(%rbp), %eax
	movl	%eax, %edx
	shrl	$31, %edx
	addl	%edx, %eax
	sarl	%eax
	cmpl	%eax, -4(%rbp)
	jl	.L3
	nop
	nop
	addq	$16, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
.LC1:
	.ascii "%f ms\12\0"
	.text
	.globl	main
	.def	main;	.scl	2;	.type	32;	.endef
	.seh_proc	main
main:
	pushq	%rbp
	.seh_pushreg	%rbp
	movl	$1000048, %eax
	call	___chkstk_ms
	subq	%rax, %rsp
	.seh_stackalloc	1000048
	leaq	128(%rsp), %rbp
	.seh_setframe	%rbp, 128
	.seh_endprologue
	call	__main
	call	clock
	movl	%eax, 999908(%rbp)
	movl	$0, 999916(%rbp)
	jmp	.L5
.L6:
	movl	999916(%rbp), %eax
	movslq	%eax, %rdx
	imulq	$1321528399, %rdx, %rdx
	shrq	$32, %rdx
	sarl	$3, %edx
	movl	%eax, %ecx
	sarl	$31, %ecx
	subl	%ecx, %edx
	imull	$26, %edx, %ecx
	subl	%ecx, %eax
	movl	%eax, %edx
	movl	%edx, %eax
	addl	$97, %eax
	movl	%eax, %edx
	movl	999916(%rbp), %eax
	cltq
	movb	%dl, -96(%rbp,%rax)
	addl	$1, 999916(%rbp)
.L5:
	cmpl	$999999, 999916(%rbp)
	jle	.L6
	movl	$0, 999912(%rbp)
	jmp	.L7
.L8:
	leaq	-96(%rbp), %rax
	movl	$1000000, %edx
	movq	%rax, %rcx
	call	reverse
	addl	$1, 999912(%rbp)
.L7:
	cmpl	$999, 999912(%rbp)
	jle	.L8
	call	clock
	movl	%eax, 999904(%rbp)
	movl	999904(%rbp), %eax
	subl	999908(%rbp), %eax
	pxor	%xmm1, %xmm1
	cvtsi2sdl	%eax, %xmm1
	movsd	.LC0(%rip), %xmm0
	mulsd	%xmm1, %xmm0
	movsd	.LC0(%rip), %xmm1
	divsd	%xmm1, %xmm0
	movapd	%xmm0, %xmm1
	movapd	%xmm1, %xmm0
	movq	%xmm1, %rdx
	leaq	.LC1(%rip), %rax
	movapd	%xmm0, %xmm1
	movq	%rax, %rcx
	call	printf
	movl	$0, %eax
	addq	$1000048, %rsp
	popq	%rbp
	ret
	.seh_endproc
	.section .rdata,"dr"
	.align 8
.LC0:
	.long	0
	.long	1083129856
	.def	__main;	.scl	2;	.type	32;	.endef
	.ident	"GCC: (Rev5, Built by MSYS2 project) 16.1.0"
	.def	clock;	.scl	2;	.type	32;	.endef
	.def	printf;	.scl	2;	.type	32;	.endef
