#ifndef INTERRUPT_H
#define INTERRUPT_H

#include <stdint.h>

/* struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)); */

// Extern function defined in assembly
extern void lidt();

void interrupt_init();

#endif
