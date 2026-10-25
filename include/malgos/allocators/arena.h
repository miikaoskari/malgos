/**
 * arena allocator with statically defined table
 */

#ifndef MLG_ALLOCATORS_ARENA
#define MLG_ALLOCATORS_ARENA

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "malgos/common/types.h"
#include <stddef.h>
#include <stdint.h>

typedef struct mlg_arena_s mlg_arena_t;
typedef struct mlg_arena_mark_s mlg_arena_mark_t;

struct mlg_arena_s
{
    unsigned char *base;
    size_t capacity;
    size_t offset;
};

struct mlg_arena_mark_s
{
    size_t offset;
};

mlg_error_t mlg_arena_init(mlg_arena_t *arena, void *buf, size_t size);
void *mlg_arena_alloc(mlg_arena_t *arena, size_t size, size_t alignment);
void *mlg_arena_alloc_zero(mlg_arena_t *arena, size_t size, size_t alignment);
mlg_error_t mlg_arena_reset(mlg_arena_t *arena);
mlg_arena_mark_t mlg_arena_mark(const mlg_arena_t *arena);
void mlg_arena_rewind(mlg_arena_t *arena, mlg_arena_mark_t *mark);
size_t mlg_arena_used(const mlg_arena_t *arena);
size_t mlg_arena_remaining(const mlg_arena_t *arena);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLG_ALLOCATORS_ARENA */
