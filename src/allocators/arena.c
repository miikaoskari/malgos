#include "malgos/allocators/arena.h"
#include "malgos/common/types.h"
#include "malgos/common/alignment.h"
#include <stddef.h>
#include <stdint.h>


mlg_error_t mlg_arena_init(mlg_arena_t *arena, void *buf, size_t size)
{
    if (!arena || !buf || !size)
    {
        return MLG_ERROR;
    }

    arena->base = buf;
    arena->capacity = size;
    arena->offset = 0;

    return MLG_OK;
}

void *mlg_arena_alloc(mlg_arena_t *arena, size_t size, size_t alignment)
{
    if (!arena || !size || !alignment)
    {
        return NULL;
    }

    /* we only allow power of two alignment
     * rounding up only allows for power of two */
    if (!mlg_is_power_of_2(alignment))
    {
        return NULL;
    }

    uintptr_t base = (uintptr_t)arena->base;
    uintptr_t cursor = base + arena->offset;
    uintptr_t aligned = mlg_align_up(cursor, alignment);
    size_t start = aligned - base;

    /* check if start address is over the arena boundary
     * and check that the size is smaller than what we have left
     * so overall, does it fit :) */
    if (start > arena->capacity || size > arena->capacity - start)
    {
        return NULL;
    }

    /* bump the offset to new address */
    arena->offset = start + size;

    return arena->base + start;
}

void *mlg_arena_alloc_zero(mlg_arena_t *arena, size_t size, size_t alignment)
{
    void *p = mlg_arena_alloc(arena, size, alignment);

    if (p)
    {
        unsigned char *pp = p;
        for (size_t i = 0; i < size; i++)
        {
            pp[i] = 0;
        }

    }

    return p;
}

mlg_error_t mlg_arena_reset(mlg_arena_t *arena)
{
    if (!arena)
    {
        return MLG_ERROR;
    }

    arena->offset = 0;

    return MLG_OK;
}

