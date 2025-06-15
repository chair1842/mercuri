#ifndef IO_H
#define IO_H

#include <stdint.h>

/**
 * Write a byte to the specified I/O port.
 */
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" 
                      : 
                      : "a"(val), "Nd"(port));
}

/**
 * Read a byte from the specified I/O port.
 */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %1, %0"
                      : "=a"(ret)
                      : "Nd"(port));
    return ret;
}

/**
 * Write a 16-bit word to the specified I/O port.
 */
static inline void outw(uint16_t port, uint16_t val) {
    __asm__ volatile ("outw %0, %1"
                      :
                      : "a"(val), "Nd"(port));
}

/**
 * Read a 16-bit word from the specified I/O port.
 */
static inline uint16_t inw(uint16_t port) {
    uint16_t ret;
    __asm__ volatile ("inw %1, %0"
                      : "=a"(ret)
                      : "Nd"(port));
    return ret;
}

#endif // IO_H
