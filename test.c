#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "allocator.h"


void mem_fill(uint8_t *addr, size_t size)
{
    for (int i = 0; i < size; ++i)
    {
        *(addr + i) = 0xFF;
    }

}


void crash_test(int count)
{
    mem_init();
    int size = 20;
    void *addrs[size];
    for (int i = 0; i < size; ++i)
    {
        addrs[i] = NULL;
    }
    int id;
    for (int i = 0; i < count; ++i)
    {
        id = rand() % size;
        size_t new_size = rand() % (256);
        if(!addrs[id])
        {
            printf("Alloc iter = %d; index = %d; new_size = %zd \n", i, id, new_size);
            addrs[id] = mem_alloc(new_size);
            if(addrs[id])
            {
              mem_fill(addrs[id], new_size);
              printf("Success\n\n");
            }
            else
            {
                printf("Failed\n\n");
            }
            continue;
        }
        else if(rand() % 2)
        {
            printf("Free iter = %d; index = %d; addr = %p\n\n", i, id, addrs[id]);
            mem_free(addrs[id]);
            addrs[id] = NULL;
        }
        else
        {
            printf("Realloc iter = %d; index = %d; prev_addr = %p; new_size = %zd\n", i, id, addrs[id], new_size);
            void* new_addr = mem_realloc(addrs[id], new_size);
            if(new_addr)
            {
                addrs[id] = new_addr;
                mem_fill(new_addr, new_size);
                printf("Success\n\n");
            }
            else 
            {
                printf("Failed\n\n");
            }
        }

    }
}


int main()
{
    crash_test(1000);
    mem_dump();
    return 0;
}
