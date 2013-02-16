#include <stdio.h>
#include <stdlib.h>
#include "allocator.h"

static uint8_t memory[SIZE];

// System utils
header_type *get_next_header(header_type *);
header_type *get_prev_header(header_type *);
void *get_userspace(header_type *);
header_type *get_header(uint8_t *);
void concat_with_next(header_type *header);


void *mem_alloc(size_t size)
{
    // Search suitable memory block
    header_type *header = (header_type*) memory;
    do
    {
        if(!header->is_busy && (header->curr_size >= size))
        {
            break;
        }
    } while((header = get_next_header(header)));

    // Can't find block with eough free space, exiting
    if(!header)
    {
        return NULL;
    }

    // Make block busy in any case
    header->is_busy = true;

    // Calculating size for new free block
    size_t new_block_size = header->curr_size - size;

    // If there is no space left for next header, use whole block
    if(new_block_size < HEADER_SIZE)
    {
        return get_userspace(header);
    }

    size_t new_free_size = new_block_size - HEADER_SIZE;

    // Modify next header link
    header_type *next_header = get_next_header(header);
    next_header->prev_size = new_free_size;

    // Modify size of founded header
    header->curr_size = size;

    // Get new next_header, create new links
    next_header = get_next_header(header);
    next_header->is_busy = false;
    next_header->curr_size = new_free_size;
    next_header->prev_size = size;

    return get_userspace(header);
}


void mem_free(void *addr)
{
    header_type *header = get_header(addr);
    header->is_busy = false;

    // If next block is free, concatenate current header with it
    if(!get_next_header(header)->is_busy)
    {
        concat_with_next(header);
    }

    header_type *prev_header = get_prev_header(header);
    // If prev block is free, concatenate current header with it
    if(!prev_header->is_busy)
    {
        concat_with_next(prev_header);
    }
}


void init()
{
    header_type *headers = (header_type*) memory;
    // init first header as busy one with size = 0
    headers[0].is_busy = true;
    headers[0].curr_size = 0;
    headers[0].prev_size = 0;

    // init second header as free one with max available size
    header_type *free_header = get_next_header(headers);
    free_header->is_busy = false;
    free_header->curr_size = SIZE - HEADER_SIZE * 3;
    free_header->prev_size = 0;

    // init last header as busy one with size = 0
    header_type *last_header = get_next_header(free_header);
    last_header->is_busy = true;
    last_header->curr_size = 0;
    last_header->prev_size = free_header->curr_size;
}


void mem_dump()
{
    printf("_______________________________________________________\n");
    header_type *header = (header_type*) memory;
    do
    {
        printf("is_busy:\t%u\n", header->is_busy);
        printf("curr_size:\t%zu\n", header->curr_size);
        printf("prev_size:\t%zu\n", header->prev_size);
        printf("--------------------\n");
    } while((header = get_next_header(header)));
    printf("_______________________________________________________\n");
}


header_type *get_next_header(header_type *header)
{
    uint8_t *offset = (uint8_t*) header;
    header_type *next_header = (header_type*) (offset + HEADER_SIZE + header->curr_size);

    // check the last one
    if((uint8_t*) next_header >= memory + SIZE)
    {
        return NULL;
    }
    return next_header;
}


header_type *get_prev_header(header_type *header)
{
    uint8_t *offset = (uint8_t*) header;
    header_type *prev_header = (header_type*) (offset - header->prev_size - HEADER_SIZE);

    // check the first one
    if((uint8_t*) prev_header < memory)
    {
        return NULL;
    }
    return prev_header;
}


// Concatenate given header with next one, use is_busy flag from given header
void concat_with_next(header_type *header)
{
    header_type *next_header = get_next_header(header);
    header->curr_size += next_header->curr_size + HEADER_SIZE;

    // Modify next header link
    next_header = get_next_header(header);
    next_header->prev_size = header->curr_size;
}

    
void *get_userspace(header_type *header)
{
    return ((uint8_t*) header) + HEADER_SIZE;
}


header_type *get_header(uint8_t *addr)
{
    return (header_type*) (addr - HEADER_SIZE);
}
