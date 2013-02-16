#include <stdio.h>
#include <stdlib.h>
#include "allocator.h"

static uint8_t memory[SIZE];

// System utils
header_type *get_next_header(header_type *);
void *get_userspace(header_type *);


void *mem_alloc(size_t size)
{
    // Search suitable memory block
    header_type *header = (header_type*) memory;
    do
    {
        if(!header->is_busy && (header->curr_size >= (size + HEADER_SIZE)))
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
    size_t new_block_size = header->curr_size - size - HEADER_SIZE;

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
    header_type *header = (header_type*) memory;
    do
    {
        printf("is_busy:\t%u\n", header->is_busy);
        printf("curr_size:\t%zu\n", header->curr_size);
        printf("prev_size:\t%zu\n\n", header->prev_size);
    } while((header = get_next_header(header)));
}


header_type *get_next_header(header_type *curr_header)
{
    uint8_t *curr_offset = (uint8_t*) curr_header;
    header_type *get_next_header = (header_type*) (curr_offset + HEADER_SIZE + curr_header->curr_size);

    // check the last one
    if((uint8_t*) get_next_header >= memory + SIZE)
    {
        return NULL;
    }
    return get_next_header;
}

    
void *get_userspace(header_type *header)
{
    return ((uint8_t*) header) + HEADER_SIZE;
}
