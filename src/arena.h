#ifndef ARENA_H
#define ARENA_H


/*
* arena.h and arena.c provide an interface for a memory arena. Memory
* arenas are wrapped around a arena_t struct, which contains three
* members: `start` which is a pointer to the first byte in the arena,
* `capacity` which is the number of bytes in the arena, and `offset`
* which is a pointer to the address where the next byte would be
* written.
*
* Callers to this interface should not modify arena_t. This struct is
* meant to be used exclusively by the functions in this module. Instead,
* callers must interact with this library via the public functions 
* provided.
*/

#include <stdlib.h>

typedef struct { 
    const char *start;
    size_t capacity;
    size_t offset;
} arena_t;


/*
* Creates an arena of `capacity` bytes and returns a pointer to an arena_t
* object where `arena_t.start` will point to the first byte in the arena upon
* success.
*
* The arena_t struct pointed at by the returned pointer lives in the heap.
* Calling `free_arena` will free the arena's memory and the arena_t struct.
*
* NULL is returned upon failure.
*/
arena_t *create_arena(size_t capacity);


/*
* Frees a memory arena and its parent arena_t struct 
*/
void free_arena(arena_t *arena);


/*
* Writes `sz` bytes from the address `src` to `arena`. In other words,
* this function allocates an object of `sz` bytes pointer at by `src`
* to the memory arena owned by the `arena` struct.
*
* If the arena cannot fit the new object, the arena gets resized 
* automatically.
*
* It returns a pointer to the first byte of the new object in the arena
* upon success and NULL upon failure.
*/
char *awrite(const char *src, size_t sz, arena_t *arena);


#endif
