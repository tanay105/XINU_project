/* zfunction.S */

.text
	.globl zfunction

zfunction:
        push   %ebp
        mov    %esp, %ebp
	mov    8(%ebp), %eax
	andl   $0xfffc01ff, %eax
	shrl   $8, %eax
	orl    $0xff000000, %eax
	leave
	ret
