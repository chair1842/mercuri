    .text
    .globl switch_thread
    .type switch_thread, @function
switch_thread:
    # Arguments on the stack:
    #   old_sp_ptr at 4(%esp)
    #   new_sp     at 8(%esp)

    movl 4(%esp), %eax     # eax = old_sp_ptr
    movl 8(%esp), %ebx     # ebx = new_sp

    movl %esp, (%eax)      # *old_sp_ptr = esp
    movl %ebx, %esp        # esp = new_sp

    popal                  # restore edi, esi, ebp, esp(dummy), ebx, edx, ecx, eax
    iret                   # pop eip/cs/eflags and resume new thread

    .size switch_thread, .-switch_thread
