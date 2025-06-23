// pic.h
#ifndef PIC_H
#define PIC_H

#include <stdint.h>

// Master PIC
#define PIC1_CMD  0x20
#define PIC1_DATA (PIC1_CMD + 1)
// Slave PIC
#define PIC2_CMD  0xA0
#define PIC2_DATA (PIC2_CMD + 1)

// Initialization Control Words
#define ICW1_INIT 0x10  // Initialize PIC
#define ICW1_ICW4 0x01  // Tell it we’ll send ICW4
#define ICW4_8086 0x01  // 8086/88 mode

// End-Of-Interrupt command code
#define PIC_EOI   0x20

void pic_remap(int offset1, int offset2);
void pic_send_eoi(uint8_t irq);
void pic_set_mask(uint8_t irq_line);
void pic_clear_mask(uint8_t irq_line);

#endif // PIC_H
