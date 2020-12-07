.section .text
.globl   kefel
kefel: movl %edi,%eax
shll $1, %eax
neg %eax
movl %edi,%ebx
shll $4, %ebx
subl %ebx,%eax
movl %edi,%ebx
movl %edi,%ecx
shll $6, %ebx
shll $8, %ecx
subl %ecx,%ebx
addl %ebx,%eax
movl %edi,%ebx
shll $10, %ebx
subl %ebx,%eax
ret
