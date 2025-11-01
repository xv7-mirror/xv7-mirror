.globl _start
_start:
    pushl 8(%esp)     # Push argv
    pushl 8(%esp)     # Push argc
    
    call main
    addl $8, %esp
    
    movl %eax, %ebx
    call exit