# coucou1
.globl main
 main: 
# prologue
pushq %rbp # save %rbp on the stack
movq %rsp, %rbp # define %rbp for the current function
    movl $42, %eax
# epilogue
popq %rbp # restore %rbp from the stack
    ret
