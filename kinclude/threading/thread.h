// thread.h
#ifndef THREAD_H
#define THREAD_H

#include <stdint.h>

#define STACK_SIZE 4096

typedef struct thread {
    uint32_t*    stack_ptr;    // Saved ESP
    void        (*entry)(void);
    struct thread* next;
} thread_t;

// The “current” running thread
extern thread_t* current_thread;

// Initialize scheduler (must be called once)
void scheduler_init(void);

// Create a new thread and add to the run queue
void thread_create(thread_t* tcb, void (*entry_point)(void));

// Yield explicitly (optional)
void thread_yield(void);

#endif // THREAD_H
