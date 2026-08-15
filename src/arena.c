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
* 
*/
static char *resize_memory(char *mem, size_t new_capacity) {
    if (!mem) {
        return NULL;
    }

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

    char *mem = (char *)malloc(capacity);
    if (!mem) {
        free(arena);
        return NULL;
    }

    arena->start = mem;
    arena->capacity = capacity;
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


char *awrite(const char *src, size_t sz, arena_t *arena) {
    if (!arena || !src) {
        return NULL;
    }

    /* Resize if neeed */
    if (get_open_bytes(arena) < sz) { /* will likely need to change to account for alingment */
        size_t new_capacity = 2 * arena->capacity + sz;
        char *new_mem = resize_memory(arena->start, new_capacity);
        if (!new_mem) {
            return NULL;
        }
        arena->start = new_mem;
        arena->capacity = new_capacity;
    }

    char *dst = arena->start + arena->offset;
    memcpy(dst, src, sz);

    return dst;
}
