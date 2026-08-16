#include "arena.h"

#include <stdlib.h>
#include <string.h>


/*
* Returns the number of available bytes in the arena.
*/
static size_t get_open_bytes(const arena_t *arena) {
    if (!arena) {
        return 0;
    }
    return arena->capacity - arena->offset;
}


/*
* Resizes the memory block pointed at by `mem` to a size of `new_capacity`.
* It returns a pointer to the new memory block upong success and NULL upon
* failure. If the reallocation fails, note that the original memory block
* remains completely valid and unchanged.
*/
static char *resize_memory(char *mem, size_t new_capacity) {
    if (!mem) {
        return NULL;
    }

    /* The C standard guarnatees realloc returns an aligned address that is
    * suitable for all data types (i.e. realloc will returns an address aligned
    * to ALIGNMENT). */
    char *new_mem = (char *)realloc(mem, new_capacity);
    if (!new_mem) {
        return NULL;
    }
    return new_mem;
}



arena_t *create_arena(size_t capacity) {
    if (!capacity) {
        return NULL;
    }

    arena_t *arena = (arena_t *)malloc(sizeof(arena_t));
    if (!arena) {
        return NULL;
    }

    size_t aligned_capacity = ALIGN_UP(capacity);
    char *mem = (char *)aligned_alloc(ALIGNMENT, aligned_capacity);
    if (!mem) {
        free(arena);
        return NULL;
    }

    memset(mem, 0, aligned_capacity);
    arena->start = mem;
    arena->capacity = aligned_capacity;
    arena->offset = 0;

    return arena;
}


void free_arena(arena_t *arena) {
    if (!arena || !arena->start) {
        return;
    }
    free(arena->start);
    free(arena);
}


size_t awrite(const char *src, size_t sz, arena_t *arena) {
    if (!arena || !src || !sz) {
        return NULL;
    }

    /* Resize if neeed */
    if (get_open_bytes(arena) < sz) { /* will likely need to change to account for alingment */
        size_t new_capacity = ALIGN_UP(2 * arena->capacity + sz);
        char *new_mem = resize_memory(arena->start, new_capacity);
        if (!new_mem) {
            return NULL;
        }
        arena->start = new_mem;
        arena->capacity = new_capacity;
    }

    /* Write `sz` bytes at the current offset which is guaranteed to be an aligned value */
    size_t current_offset = arena->offset;
    memcpy(arena->start + current_offset, src, sz);
    arena->offset = ALIGN_UP(current_offset + sz);
    return current_offset;
}
