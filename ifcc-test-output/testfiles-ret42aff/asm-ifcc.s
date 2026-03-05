.globl main
 main: 
# prologue
pushq %rbp # save %rbp on the stack
movq %rsp, %rbp # define %rbp for the current function
    movl $42, -0(%rbp)
    movl -0(%rbp), %eax
# epilogue
popq %rbp # restore %rbp from the stack
    ret
