#define SIZE 1024

void* mem_alloc(size_t size);
void mem_free(void *addr);
void *mem_realloc(void *addr, size_t size);
void mem_dump();
void mem_init();
