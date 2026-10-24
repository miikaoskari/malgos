/**
 * arena allocator with statically defined table
 */

#ifndef MLG_ALLOCATORS_ARENA
#define MLG_ALLOCATORS_ARENA

#include "malgos/common/types.h"
#include <stddef.h>
#include <stdint.h>

typedef struct mlg_arena_s mlg_arena_t;

struct mlg_arena_s
{
    unsigned char *base;
    size_t capacity;
    size_t offset;
};

mlg_error_t mlg_arena_init(mlg_arena_t *arena, void *buf, size_t size);
void *mlg_arena_alloc(mlg_arena_t *arena, size_t size, size_t alignment);
mlg_error_t mlg_arena_reset(mlg_arena_t *arena);

#endif /* MLG_ALLOCATORS_ARENA */
