// thread.c
#include <threading/thread.h>
#include <memory/memory.h>   // for kmalloc()

thread_t* current_thread = 0;
static thread_t* last_thread = 0;

void scheduler_init(void) {
    current_thread = 0;
    last_thread    = 0;
}

void thread_create(thread_t* tcb, void (*entry_point)(void)) {
    // Allocate a stack (STACK_SIZE bytes)
    uint8_t* stack_mem = kmalloc(STACK_SIZE);
    uint32_t* sp = (uint32_t*)(stack_mem + STACK_SIZE);

    // Make space for initial EFLAGS, CS, EIP frame that iretq would expect:
    *(--sp) = 0x200;             // EFLAGS (IF = 1)
    *(--sp) = 0x08;              // CS (kernel code segment)
    *(--sp) = (uint32_t)entry_point;  // EIP → entry_point()

    // Make space for pusha restore order: EAX,ECX,EDX,EBX,ESP,EBP,ESI,EDI
    for (int i = 0; i < 8; i++) *(--sp) = 0;

    tcb->stack_ptr = sp;
    tcb->entry     = entry_point;

    // Insert into circular run‑queue
    if (!current_thread) {
        current_thread = tcb;
        last_thread    = tcb;
        tcb->next      = tcb;
    } else {
        tcb->next     = current_thread;
        last_thread->next = tcb;
        last_thread   = tcb;
    }
}

// Called by PIT IRQ or manually to switch threads
void thread_yield(void) {
    extern void switch_thread(uint32_t** old_sp, uint32_t* new_sp);
    thread_t* old = current_thread;
    current_thread = current_thread->next;
    switch_thread(&old->stack_ptr, current_thread->stack_ptr);
}
