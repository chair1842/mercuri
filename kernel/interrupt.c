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

extern void pic_remap(int offset1, int offset2);

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

void interrupt_init(void) {
    // 1) Remap PIC: IRQs 0–15 → vectors 0x20–0x2F
    pic_remap(0x20, 0x28);

    // 2) Mask all IRQs initially
    outb(0x21, 0xFF); // Mask master PIC
    outb(0xA1, 0xFF); // Mask slave PIC

    // 3) Install IDT entries for exceptions (0–31) and IRQs (32–47)
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt_set_gate(i, (uint32_t)isr_stub_table[i], 0x08, 0x8E);
    }

    // 4) Load the IDT
    idtp.limit = sizeof(idt) - 1;
    idtp.base  = (uint32_t)&idt;
    lidt(&idtp);

    // 5) Unmask only IRQ0 (timer) and IRQ1 (keyboard)
    //    Master PIC mask port = 0x21: clear bits 0 and 1
    outb(0x21, 0xFC);  // 11111100b => IRQ0 & IRQ1 enabled
    //    Leave slave fully masked (unless you have IRQs there)
    outb(0xA1, 0xFF);

    // 6) Initialize drivers that rely on IRQs
    keyboard_init();         // drains buffer + (re)unmasks IRQ1 bit
    print_string("Keyboard driver ready\n");

    // 7) Now safe to enable interrupts
    __asm__ volatile("sti");
}