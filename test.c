#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "allocator.h"


typedef struct {
    void *addr;
    size_t size;
    uint16_t cs;
} var_type;


void mem_fill(uint8_t *addr, size_t size)
{
    for (int i = 0; i < size; ++i)
    {
        *(addr + i) = rand() % 0xFF;
    }

}


uint16_t calc_checksum(uint8_t *addr, size_t size)
{
    uint16_t cs = 0;
    for (int i = 0; i < size; ++i)
    {
        cs += *(addr + i);
    }
    return cs;

}


int crash_test(int count)
{
    mem_init();
    int size = 30;
    var_type vars[size];
    for (int i = 0; i < size; ++i)
    {
        vars[i].addr = NULL;
        vars[i].size = 0;
        vars[i].cs = 0;
    }
    int id;
    for (int i = 0; i < count; ++i)
    {
        id = rand() % size;
        size_t new_size = rand() % (128);
        if(!vars[id].addr)
        {
            void *new_addr = mem_alloc(new_size);
            if(new_addr)
            {
              printf("%d: alloc id = %d; new_addr = %p; new_size = %zd \n", i, id, new_addr, new_size);
              vars[id].addr = new_addr;
              vars[id].size = new_size;
              mem_fill(vars[id].addr, vars[id].size);
              vars[id].cs = calc_checksum(vars[id].addr, vars[id].size);
            }
            else
            {
                printf("%d alloc: failed; new_size = %zd\n", i, new_size);
            }
        }
        else if(rand() % 2)
        {
            printf("%d: free id = %d; addr = %p\n", i, id, vars[id].addr);
            mem_free(vars[id].addr);
            vars[id].cs = 0;
            vars[id].size = 0;
            vars[id].addr = NULL;
        }
        else
        {
            void* new_addr = mem_realloc(vars[id].addr, new_size);
            if(new_addr)
            {
                printf("%d: realloc id = %d; prev_addr = %p; prev_size = %zd; new_addr = %p; new_size = %zd\n",
                       i, id, vars[id].addr, vars[id].size, new_addr, new_size);
                vars[id].addr = new_addr;
                vars[id].size = new_size;
                vars[id].cs = calc_checksum(vars[id].addr, vars[id].size);
            }
            else 
            {
                printf("%d realloc: failed; new_size = %zd\n", i, new_size);
            }
        }

        // Verify checksums and memory
        for (int j = 0; j < size; ++j)
        {
            if(!vars[j].addr)
            {
                continue;
            }
            uint16_t cs = calc_checksum(vars[j].addr, vars[j].size);
            if(cs != vars[j].cs)
            {
                printf("Checksum failed(id=%d): %d != %d\n", j, cs, vars[j].cs);
                return 1;
            }
        }
        printf("Checksum OK\n\n");

        int status = mem_verify();
        if(status)
        {
            printf("Error, mem not valid. Exiting\n");
            return status;
        }


    }
    return 0;
}


int main()
{
    int status = crash_test(1000);
    mem_dump();
    return status;
}
