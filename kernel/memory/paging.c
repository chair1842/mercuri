#include <memory/paging.h>

#define PAGE_DIR_ENTRIES 1024
#define PAGE_TABLE_ENTRIES 1024

// 4KB aligned page directory and one table (for first 4MB)
__attribute__((aligned(PAGE_SIZE))) static uint32_t page_directory[PAGE_DIR_ENTRIES];
__attribute__((aligned(PAGE_SIZE))) static uint32_t first_page_table[PAGE_TABLE_ENTRIES];

void paging_init() {
    // Identity map first 4MB using one page table
    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        first_page_table[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_RW;
    }

    // Point first directory entry to page table
    page_directory[0] = ((uint32_t)first_page_table) | PAGE_PRESENT | PAGE_RW;

    // Zero the rest
    for (int i = 1; i < PAGE_DIR_ENTRIES; i++) {
        page_directory[i] = 0;
    }

    // Load page directory base register (CR3)
    asm volatile("mov %0, %%cr3" :: "r"(page_directory));

    // Enable paging (set bit 31 in CR0)
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // Set PG bit
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}
