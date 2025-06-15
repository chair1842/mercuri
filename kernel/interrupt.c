#include <stdint.h>
#include <interrupt.h>
#include <driver/keyboard.h>
#include <io.h>
#include <vga.h>

#define IDT_ENTRIES 256
struct idt_entry {
    uint16_t base_lo;
    uint16_t sel;       
    uint8_t  always0;   
    uint8_t  flags;     
    uint16_t base_hi;
} __attribute__((packed));
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

extern void (*isr_stub_table[])();  // from assembly
static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr   idtp;

void irq1_handler() {
    keyboard_poll(); // Use your existing logic
    outb(0x20, 0x20); // Send EOI to PIC
}
void irq0_handler() {
    // Timer interrupt handler logic (if needed)
    outb(0x20, 0x20); // Send EOI to PIC
}

void isr_handler_c(unsigned int vector) {
    if (vector == 0x20) {
        irq0_handler();
    }
    if (vector == 0x21) {
        irq1_handler();  // your keyboard handler
    }
    // ... other IRQs/exceptions ...
}


static void idt_set_gate(int n, uint32_t handler, uint16_t sel, uint8_t flags) {
    idt[n].base_lo  = handler & 0xFFFF;
    idt[n].base_hi  = (handler >> 16) & 0xFFFF;
    idt[n].sel      = sel;
    idt[n].always0  = 0;
    idt[n].flags    = flags /* | 0x60 for user-mode if needed */;
}

extern void lidt(struct idt_ptr* idt_ptr);

void interrupt_init() {
    extern void pic_remap(int, int);
    pic_remap(0x20, 0x28);

    // Unmask IRQ0 (timer) and IRQ1 (keyboard) on master PIC
    // PIC1 mask = 11111100b → bits0–1 = 0 (unmasked)
    outb(0x21, 0xFC);
    // Keep slave PIC fully masked until you need it
    outb(0xA1, 0xFF);

    // clear all entries
    for (int i = 0; i < IDT_ENTRIES; i++)
        idt_set_gate(i, 0, 0, 0);

    // Install stubs for ISR 0–47 (CPU exceptions + IRQs)
    for (int i = 0; i < 48; i++) {
        idt_set_gate(
            i,
            (uint32_t)isr_stub_table[i],  // the ith function pointer
            0x08,
            0x8E
        );
    }

    // load IDT
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;
    lidt(&idtp);

    /* // Initialize keyboard driver & unmask IRQ1
    keyboard_init();
    print_string("Keyboard Initialized\n"); */

    // Now enable interrupts!
    __asm__ volatile("sti");
}