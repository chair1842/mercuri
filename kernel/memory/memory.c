#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <memory/memory.h>

#define KERNEL_HEAP_START 0x200000
#define KERNEL_HEAP_SIZE  0x100000  // 1MB heap

typedef struct BlockHeader {
    size_t size;
    bool free;
    struct BlockHeader* next;
} BlockHeader;

#define BLOCK_SIZE sizeof(BlockHeader)

static uint8_t* heap_base = (uint8_t*)KERNEL_HEAP_START;
static BlockHeader* free_list = NULL;

void memory_init() {
    free_list = (BlockHeader*)heap_base;
    free_list->size = KERNEL_HEAP_SIZE - BLOCK_SIZE;
    free_list->free = true;
    free_list->next = NULL;
}

void* kmalloc(size_t size) {
    BlockHeader* current = free_list;

    while (current) {
        if (current->free && current->size >= size) {
            if (current->size >= size + BLOCK_SIZE + 4) {
                // Split block
                BlockHeader* new_block = (BlockHeader*)((uint8_t*)current + BLOCK_SIZE + size);
                new_block->size = current->size - size - BLOCK_SIZE;
                new_block->free = true;
                new_block->next = current->next;
                current->next = new_block;
                current->size = size;
            }

            current->free = false;
            return (uint8_t*)current + BLOCK_SIZE;
        }
        current = current->next;
    }

    return NULL; // Out of memory
}

void kfree(void* ptr) {
    if (!ptr) return;

    BlockHeader* block = (BlockHeader*)((uint8_t*)ptr - BLOCK_SIZE);
    block->free = true;

    // Coalesce adjacent free blocks
    BlockHeader* current = free_list;
    while (current) {
        if (current->free && current->next && current->next->free) {
            current->size += BLOCK_SIZE + current->next->size;
            current->next = current->next->next;
        } else {
            current = current->next;
        }
    }
}
