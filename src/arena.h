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
*
* The arena is aligned using max_align_t, an alignment value defined in
* stddef.h which equals the largest alignment required to align every 
* object type in a system. Note that an alignment of, say, 16 correctly
* aligns types with alignments of 1, 2, 4, 8 because these are multiples 
* 16. max_align_t is sort of like 16 in this example. It is a value that
* large enough to align every other possible type.
*
* NEEDSWORK: using max_align_t adds a lot of padding bytes to the 
* arena's memory block. We could reduce memory usage by using alignof()
* and having callers pass an alignment value that is specific to the type
* they want to allocate.
*/

#include <stdlib.h>
#include <stddef.h>
#include <stdalign.h>


/* Maximum aligned required to allocate any type */
#define ALIGNMENT alignof(max_align_t)

/* Align `val` up to ALIGNMENT */
#define ALIGN_UP(val) (((val) + ALIGNMENT - 1) & ~(ALIGNMENT - 1))

typedef struct { 
    char *start;
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
* It returns an integer representing the offset from `start` where the
* object was allocated.
*/
size_t awrite(const char *src, size_t sz, arena_t *arena);


#endif
