#include "arena.h"

#include <stdlib.h>


arena_t *create_arena(size_t capacity) {
    arena_t *arena = malloc(sizeof(arena_t));
    if (!arena) {
        return NULL;
    }

    char *mem = malloc(capacity);
    if (!mem) {
        free(arena);
        return NULL;
    }

    arena->start = mem;
    arena->capacity = capacity;
    arena->offset = 0;

    return arena;
}
