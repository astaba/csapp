; =========================================================================
; Created on: <Thu Apr 23 14:08:46 +01 2026> 
; Time-stamp: <Thu Apr 23 14:44:03 +01 2026 by owner> 
; Author    : owner
; Desc      : ~/coding/c_prog/csapp/conc/badcnt.asm -
;
; Assembly of [[file:badcnt.c::int main]]
; =========================================================================
	.file	"badcnt.c"
	.text
	.globl	cnt
	.bss
	.align 8
	.type	cnt, @object
	.size	cnt, 8
cnt:
	.zero	8
	.section	.rodata
.LC0:
	.string	"usage: %s <niters>\n"
.LC1:
	.string	"BOOM! cnt=%ld\n"
.LC2:
	.string	"OK cnt=%ld\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%fs:40, %rax
	movq	%rax, -8(%rbp)
	xorl	%eax, %eax
	cmpl	$2, -36(%rbp)
	je	.L2
	movq	-48(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, %rsi
	leaq	.LC0(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %edi
	call	exit@PLT
.L2:
	movq	-48(%rbp), %rax
	addq	$8, %rax
	movq	(%rax), %rax
	movq	%rax, %rdi
	call	atoi@PLT
	cltq
	movq	%rax, -32(%rbp)
	leaq	-32(%rbp), %rdx
	leaq	-24(%rbp), %rax
	movq	%rdx, %rcx
	leaq	thread(%rip), %rdx
	movl	$0, %esi
	movq	%rax, %rdi
	call	Pthread_create@PLT
	leaq	-32(%rbp), %rdx
	leaq	-16(%rbp), %rax
	movq	%rdx, %rcx
	leaq	thread(%rip), %rdx
	movl	$0, %esi
	movq	%rax, %rdi
	call	Pthread_create@PLT
	movq	-24(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	Pthread_join@PLT
	movq	-16(%rbp), %rax
	movl	$0, %esi
	movq	%rax, %rdi
	call	Pthread_join@PLT
	movq	-32(%rbp), %rax
	leaq	(%rax,%rax), %rdx
	movq	cnt(%rip), %rax
	cmpq	%rax, %rdx
	je	.L3
	movq	cnt(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC1(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
	jmp	.L4
.L3:
	movq	cnt(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC2(%rip), %rax
	movq	%rax, %rdi
	movl	$0, %eax
	call	printf@PLT
.L4:
	movl	$0, %edi
	call	exit@PLT
	.cfi_endproc
.LFE6:
	.size	main, .-main
	.globl	thread
	.type	thread, @function
thread:				; thread routine
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -24(%rbp)	; declare *vargp: 3rd Q-slot down stack frame
	movq	-24(%rbp), %rax	; move niters.m address into RAX
	movq	(%rax), %rax	; dereference niters.m value into RAX
	movq	%rax, -8(%rbp)	; declare niters.i: 1st Q-slot down stack frame
	movq	$0, -16(%rbp)	; declare i: 2nd Q-slot down stack frame
	jmp	.L7
.L8:				; Body of each For loop iteration
	;; WARN: The 3 following instructions are subjects to RACE CONDITION.
	movq	cnt(%rip), %rax	; Load.		NOTE: Read cnt into RAX
	addq	$1, %rax	; Update. 	NOTE: Increment RAX
	movq	%rax, cnt(%rip)	; Store.	NOTE: Write RAX back to cnt
	;; WARN: It shows: C "cnt++" is not atomic.
	addq	$1, -16(%rbp)	; Update loop counter by +1
.L7:
	movq	-16(%rbp), %rax	; move i to RAX
	cmpq	-8(%rbp), %rax	; subtract niters.i from RAX (i - niters.i)
	jl	.L8		; While < 0 iterate again from .L8
	movl	$0, %eax	; define the returned NULL
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	thread, .-thread
	.ident	"GCC: (Ubuntu 13.3.0-6ubuntu2~24.04.1) 13.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	1f - 0f
	.long	4f - 1f
	.long	5
0:
	.string	"GNU"
1:
	.align 8
	.long	0xc0000002
	.long	3f - 2f
2:
	.long	0x3
3:
	.align 8
4:
