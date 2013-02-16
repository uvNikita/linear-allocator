#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#define SIZE 1024
#define HEADER_SIZE sizeof(header_type)

typedef struct {
    uint8_t is_busy;
    size_t curr_size;
    size_t prev_size;
} header_type;

void* mem_alloc(size_t size);
void mem_free(void *addr);
void *mem_realloc(void *addr, size_t size);
void mem_dump();
void mem_init();
