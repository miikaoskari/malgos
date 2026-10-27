#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "malgos/allocators/arena.h"
#include "malgos/common/types.h"
#include "unity.h"
#include "unity_internals.h"

#define BUF_SIZE 64

/* 16-byte aligned so offsets in the tests are predictable */
static _Alignas(16) unsigned char buf[BUF_SIZE];
static mlg_arena_t arena;

void setUp(void)
{
    memset(buf, 0, sizeof buf);
    TEST_ASSERT_EQUAL(MLG_OK, mlg_arena_init(&arena, buf, sizeof buf));
}

void tearDown(void)
{
}

static int is_aligned(const void *p, size_t alignment)
{
    return ((uintptr_t)p & (alignment - 1)) == 0;
}

/* init */

void test_arena_init_sets_fields(void)
{
    TEST_ASSERT_EQUAL_PTR(buf, arena.base);
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE, arena.capacity);
    TEST_ASSERT_EQUAL_size_t(0, arena.offset);
}

void test_arena_init_rejects_invalid_args(void)
{
    mlg_arena_t a;
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_arena_init(NULL, buf, sizeof buf));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_arena_init(&a, NULL, sizeof buf));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_arena_init(&a, buf, 0));
}

/* basic allocation */

void test_arena_first_alloc_returns_base(void)
{
    void *p = mlg_arena_alloc(&arena, 8, 1);
    TEST_ASSERT_EQUAL_PTR(buf, p);
    TEST_ASSERT_EQUAL_size_t(8, arena.offset);
}

void test_arena_consecutive_allocs_bump_offset(void)
{
    unsigned char *a = mlg_arena_alloc(&arena, 3, 1);
    unsigned char *b = mlg_arena_alloc(&arena, 3, 1);
    unsigned char *c = mlg_arena_alloc(&arena, 3, 1);

    TEST_ASSERT_EQUAL_PTR(buf + 0, a);
    TEST_ASSERT_EQUAL_PTR(buf + 3, b);
    TEST_ASSERT_EQUAL_PTR(buf + 6, c);
    TEST_ASSERT_EQUAL_size_t(9, arena.offset);
}

/* the walkthrough from the notes: string, int, double */
void test_arena_padding_walkthrough(void)
{
    void *s = mlg_arena_alloc(&arena, 5, 1);
    void *i = mlg_arena_alloc(&arena, 4, 4);
    void *d = mlg_arena_alloc(&arena, 8, 8);

    TEST_ASSERT_EQUAL_PTR(buf + 0, s);
    TEST_ASSERT_EQUAL_PTR(buf + 8, i);  /* 3 bytes of padding after s */
    TEST_ASSERT_EQUAL_PTR(buf + 16, d); /* 4 bytes of padding after i */
    TEST_ASSERT_EQUAL_size_t(24, arena.offset);
}

void test_arena_typed_allocs_are_aligned(void)
{
    mlg_arena_alloc(&arena, 1, 1); /* knock the offset off alignment */

    double *d = mlg_arena_alloc(&arena, sizeof(double), _Alignof(double));
    TEST_ASSERT_NOT_NULL(d);
    TEST_ASSERT_TRUE(is_aligned(d, _Alignof(double)));
    *d = 3.5; /* must be usable as a double */
    TEST_ASSERT_EQUAL_DOUBLE(3.5, *d);
}

void test_arena_every_power_of_two_alignment(void)
{
    for (size_t align = 1; align <= 32; align <<= 1)
    {
        mlg_arena_reset(&arena);
        mlg_arena_alloc(&arena, 1, 1); /* offset 1: misaligned for all align > 1 */

        void *p = mlg_arena_alloc(&arena, 1, align);
        TEST_ASSERT_NOT_NULL(p);
        TEST_ASSERT_TRUE(is_aligned(p, align));
    }
}

/* alignment is computed on the address, not the offset */
void test_arena_unaligned_base_still_returns_aligned_pointers(void)
{
    mlg_arena_t a;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_arena_init(&a, buf + 1, sizeof buf - 1));

    void *p = mlg_arena_alloc(&a, 8, 8);
    TEST_ASSERT_NOT_NULL(p);
    TEST_ASSERT_TRUE(is_aligned(p, 8));
    TEST_ASSERT_EQUAL_PTR(buf + 8, p);
}

void test_arena_allocations_do_not_overlap(void)
{
    unsigned char *a = mlg_arena_alloc(&arena, 10, 1);
    unsigned char *b = mlg_arena_alloc(&arena, 10, 4);
    unsigned char *c = mlg_arena_alloc(&arena, 10, 8);
    TEST_ASSERT_NOT_NULL(a);
    TEST_ASSERT_NOT_NULL(b);
    TEST_ASSERT_NOT_NULL(c);

    memset(a, 0xAA, 10);
    memset(b, 0xBB, 10);
    memset(c, 0xCC, 10);

    for (int i = 0; i < 10; i++)
    {
        TEST_ASSERT_EQUAL_HEX8(0xAA, a[i]);
        TEST_ASSERT_EQUAL_HEX8(0xBB, b[i]);
        TEST_ASSERT_EQUAL_HEX8(0xCC, c[i]);
    }
}

/* capacity limits */

void test_arena_exact_fit_succeeds(void)
{
    void *p = mlg_arena_alloc(&arena, BUF_SIZE, 1);
    TEST_ASSERT_EQUAL_PTR(buf, p);
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE, arena.offset);
}

void test_arena_alloc_when_full_fails(void)
{
    TEST_ASSERT_NOT_NULL(mlg_arena_alloc(&arena, BUF_SIZE, 1));
    TEST_ASSERT_NULL(mlg_arena_alloc(&arena, 1, 1));
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE, arena.offset);
}

void test_arena_too_large_fails_and_leaves_offset(void)
{
    mlg_arena_alloc(&arena, 10, 1);
    TEST_ASSERT_NULL(mlg_arena_alloc(&arena, BUF_SIZE, 1));
    TEST_ASSERT_EQUAL_size_t(10, arena.offset);
}

/* fits without padding, but not once the padding is added */
void test_arena_padding_pushes_alloc_over_capacity(void)
{
    mlg_arena_alloc(&arena, BUF_SIZE - 9, 1); /* offset 55, 9 bytes left */

    /* 8 bytes would fit at 55, but aligning to 8 moves it to 56..64: fits */
    TEST_ASSERT_NOT_NULL(mlg_arena_alloc(&arena, 8, 8));

    mlg_arena_reset(&arena);
    mlg_arena_alloc(&arena, BUF_SIZE - 9, 1);

    /* 9 bytes aligned to 8 would need 56..65: one byte too many */
    TEST_ASSERT_NULL(mlg_arena_alloc(&arena, 9, 8));
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE - 9, arena.offset);
}

/* the fit check must not overflow on huge sizes */
void test_arena_huge_size_fails(void)
{
    mlg_arena_alloc(&arena, 1, 1);
    TEST_ASSERT_NULL(mlg_arena_alloc(&arena, SIZE_MAX, 1));
    TEST_ASSERT_NULL(mlg_arena_alloc(&arena, SIZE_MAX - 1, 1));
    TEST_ASSERT_EQUAL_size_t(1, arena.offset);
}

void test_arena_fill_with_single_bytes(void)
{
    size_t count = 0;
    while (mlg_arena_alloc(&arena, 1, 1) != NULL)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE, count);
}

/* invalid arguments */

void test_arena_alloc_rejects_null_arena(void)
{
    TEST_ASSERT_NULL(mlg_arena_alloc(NULL, 8, 8));
}

void test_arena_alloc_zero_size_returns_aligned_pointer(void)
{
    void *p = mlg_arena_alloc(&arena, 0, 8);
    TEST_ASSERT_EQUAL_PTR(buf, p);
    TEST_ASSERT_EQUAL_size_t(0, arena.offset);
}

void test_arena_alloc_zero_size_only_consumes_padding(void)
{
    TEST_ASSERT_NOT_NULL(mlg_arena_alloc(&arena, 3, 1));
    unsigned char *p = mlg_arena_alloc(&arena, 0, 8);
    TEST_ASSERT_EQUAL_PTR(buf + 8, p);
    TEST_ASSERT_EQUAL_size_t(8, arena.offset);
}

void test_arena_alloc_zero_size_succeeds_when_full(void)
{
    TEST_ASSERT_NOT_NULL(mlg_arena_alloc(&arena, BUF_SIZE, 1));
    TEST_ASSERT_EQUAL_PTR(buf + BUF_SIZE, mlg_arena_alloc(&arena, 0, 1));
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE, arena.offset);
}

void test_arena_alloc_rejects_non_power_of_two_alignment(void)
{
    const size_t bad[] = { 0, 3, 5, 6, 7, 12, 24 };
    for (size_t i = 0; i < sizeof bad / sizeof bad[0]; i++)
    {
        TEST_ASSERT_NULL(mlg_arena_alloc(&arena, 1, bad[i]));
    }
    TEST_ASSERT_EQUAL_size_t(0, arena.offset);
}

/* reset */

void test_arena_reset_rewinds_offset_and_keeps_capacity(void)
{
    mlg_arena_alloc(&arena, 20, 4);
    TEST_ASSERT_EQUAL(MLG_OK, mlg_arena_reset(&arena));
    TEST_ASSERT_EQUAL_size_t(0, arena.offset);
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE, arena.capacity);
    TEST_ASSERT_EQUAL_PTR(buf, arena.base);
}

void test_arena_alloc_after_reset_reuses_memory(void)
{
    void *first = mlg_arena_alloc(&arena, 16, 8);
    mlg_arena_reset(&arena);
    void *again = mlg_arena_alloc(&arena, 16, 8);
    TEST_ASSERT_EQUAL_PTR(first, again);
}

void test_arena_full_capacity_available_after_reset(void)
{
    TEST_ASSERT_NOT_NULL(mlg_arena_alloc(&arena, BUF_SIZE, 1));
    mlg_arena_reset(&arena);
    TEST_ASSERT_NOT_NULL(mlg_arena_alloc(&arena, BUF_SIZE, 1));
}

void test_arena_reset_rejects_null(void)
{
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_arena_reset(NULL));
}

/* alloc_zero */

#define DIRTY 0xEE

static void assert_bytes(const unsigned char *p, size_t n, unsigned char value)
{
    for (size_t i = 0; i < n; i++)
    {
        TEST_ASSERT_EQUAL_HEX8_MESSAGE(value, p[i], "unexpected byte value");
    }
}

/* the main reason alloc_zero exists: memory reused after reset is dirty */
void test_arena_alloc_zero_clears_reused_memory(void)
{
    unsigned char *first = mlg_arena_alloc(&arena, 32, 1);
    memset(first, DIRTY, 32);

    mlg_arena_reset(&arena);

    unsigned char *again = mlg_arena_alloc_zero(&arena, 32, 1);
    TEST_ASSERT_EQUAL_PTR(first, again);
    assert_bytes(again, 32, 0);
}

void test_arena_alloc_zero_clears_whole_capacity(void)
{
    memset(buf, DIRTY, sizeof buf);

    unsigned char *p = mlg_arena_alloc_zero(&arena, BUF_SIZE, 1);
    TEST_ASSERT_EQUAL_PTR(buf, p);
    assert_bytes(p, BUF_SIZE, 0);
}

/* only the allocation is zeroed: not the padding before it, not the bytes after */
void test_arena_alloc_zero_only_touches_its_own_bytes(void)
{
    memset(buf, DIRTY, sizeof buf);

    mlg_arena_alloc(&arena, 1, 1); /* offset 1 */
    unsigned char *p = mlg_arena_alloc_zero(&arena, 4, 4);

    TEST_ASSERT_EQUAL_PTR(buf + 4, p);
    assert_bytes(buf + 1, 3, DIRTY); /* padding untouched */
    assert_bytes(buf + 4, 4, 0);     /* the allocation */
    assert_bytes(buf + 8, BUF_SIZE - 8, DIRTY); /* rest untouched */
}

struct zero_node
{
    struct zero_node *next;
    int count;
    double weight;
    unsigned char flags[3];
};

void test_arena_alloc_zero_struct_fields_start_empty(void)
{
    memset(buf, DIRTY, sizeof buf);

    struct zero_node *n = mlg_arena_alloc_zero(
        &arena, sizeof(struct zero_node), _Alignof(struct zero_node));

    TEST_ASSERT_NOT_NULL(n);
    TEST_ASSERT_TRUE(is_aligned(n, _Alignof(struct zero_node)));
    TEST_ASSERT_NULL(n->next);
    TEST_ASSERT_EQUAL_INT(0, n->count);
    TEST_ASSERT_EQUAL_DOUBLE(0.0, n->weight);
    assert_bytes(n->flags, sizeof n->flags, 0);
}

/* same placement and offset as alloc would give */
void test_arena_alloc_zero_matches_alloc_layout(void)
{
    mlg_arena_t plain;
    static _Alignas(16) unsigned char plain_buf[BUF_SIZE];
    mlg_arena_init(&plain, plain_buf, sizeof plain_buf);

    const size_t sizes[] = { 5, 4, 8, 3, 16 };
    const size_t aligns[] = { 1, 4, 8, 2, 16 };

    for (size_t i = 0; i < sizeof sizes / sizeof sizes[0]; i++)
    {
        unsigned char *z = mlg_arena_alloc_zero(&arena, sizes[i], aligns[i]);
        unsigned char *a = mlg_arena_alloc(&plain, sizes[i], aligns[i]);
        TEST_ASSERT_NOT_NULL(z);
        TEST_ASSERT_NOT_NULL(a);
        TEST_ASSERT_EQUAL_size_t((size_t)(a - plain_buf), (size_t)(z - buf));
        TEST_ASSERT_EQUAL_size_t(plain.offset, arena.offset);
    }
}

/* a failed alloc_zero must not write anything */
void test_arena_alloc_zero_failure_leaves_buffer_untouched(void)
{
    memset(buf, DIRTY, sizeof buf);
    mlg_arena_alloc(&arena, 60, 1);

    TEST_ASSERT_NULL(mlg_arena_alloc_zero(&arena, 8, 1));
    TEST_ASSERT_EQUAL_size_t(60, arena.offset);
    assert_bytes(buf, BUF_SIZE, DIRTY);
}

void test_arena_alloc_zero_huge_size_fails(void)
{
    memset(buf, DIRTY, sizeof buf);
    TEST_ASSERT_NULL(mlg_arena_alloc_zero(&arena, SIZE_MAX, 1));
    TEST_ASSERT_EQUAL_size_t(0, arena.offset);
    assert_bytes(buf, BUF_SIZE, DIRTY);
}

void test_arena_alloc_zero_rejects_invalid_args(void)
{
    TEST_ASSERT_NULL(mlg_arena_alloc_zero(NULL, 8, 8));
    TEST_ASSERT_NULL(mlg_arena_alloc_zero(&arena, 8, 0));
    TEST_ASSERT_NULL(mlg_arena_alloc_zero(&arena, 8, 6));
    TEST_ASSERT_EQUAL_size_t(0, arena.offset);
}

/* used / remaining */

void test_arena_used_and_remaining_on_fresh_arena(void)
{
    TEST_ASSERT_EQUAL_size_t(0, mlg_arena_used(&arena));
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE, mlg_arena_remaining(&arena));
}

void test_arena_used_and_remaining_track_allocs(void)
{
    mlg_arena_alloc(&arena, 10, 1);
    TEST_ASSERT_EQUAL_size_t(10, mlg_arena_used(&arena));
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE - 10, mlg_arena_remaining(&arena));
}

/* alignment padding counts as used space */
void test_arena_used_includes_padding(void)
{
    mlg_arena_alloc(&arena, 3, 1);
    mlg_arena_alloc(&arena, 8, 8);
    TEST_ASSERT_EQUAL_size_t(16, mlg_arena_used(&arena));
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE - 16, mlg_arena_remaining(&arena));
}

void test_arena_used_plus_remaining_is_capacity(void)
{
    const size_t sizes[] = { 1, 7, 3, 12, 5 };
    const size_t aligns[] = { 1, 4, 8, 16, 2 };
    for (size_t i = 0; i < sizeof sizes / sizeof sizes[0]; i++)
    {
        TEST_ASSERT_NOT_NULL(mlg_arena_alloc(&arena, sizes[i], aligns[i]));
        TEST_ASSERT_EQUAL_size_t(BUF_SIZE,
            mlg_arena_used(&arena) + mlg_arena_remaining(&arena));
    }
}

void test_arena_used_and_remaining_when_full(void)
{
    mlg_arena_alloc(&arena, BUF_SIZE, 1);
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE, mlg_arena_used(&arena));
    TEST_ASSERT_EQUAL_size_t(0, mlg_arena_remaining(&arena));
}

void test_arena_failed_alloc_does_not_change_used(void)
{
    mlg_arena_alloc(&arena, 10, 1);
    TEST_ASSERT_NULL(mlg_arena_alloc(&arena, BUF_SIZE, 1));
    TEST_ASSERT_EQUAL_size_t(10, mlg_arena_used(&arena));
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE - 10, mlg_arena_remaining(&arena));
}

void test_arena_reset_restores_used_and_remaining(void)
{
    mlg_arena_alloc(&arena, 20, 4);
    mlg_arena_reset(&arena);
    TEST_ASSERT_EQUAL_size_t(0, mlg_arena_used(&arena));
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE, mlg_arena_remaining(&arena));
}

void test_arena_used_rejects_null(void)
{
    TEST_ASSERT_EQUAL_size_t(0, mlg_arena_used(NULL));
}

void test_arena_remaining_rejects_null(void)
{
    TEST_ASSERT_EQUAL_size_t(0, mlg_arena_remaining(NULL));
}

/* offset past capacity must not wrap around to a huge value */
void test_arena_remaining_does_not_underflow_on_corrupt_offset(void)
{
    arena.offset = BUF_SIZE + 1;
    TEST_ASSERT_EQUAL_size_t(0, mlg_arena_remaining(&arena));
}

/* an uninitialized (zeroed) arena has nothing used and nothing left */
void test_arena_used_and_remaining_on_zeroed_struct(void)
{
    mlg_arena_t empty = { 0 };
    TEST_ASSERT_EQUAL_size_t(0, mlg_arena_used(&empty));
    TEST_ASSERT_EQUAL_size_t(0, mlg_arena_remaining(&empty));
}

/* a failed init must not leave the arena looking usable */
void test_arena_failed_init_leaves_arena_untouched(void)
{
    mlg_arena_t a = { 0 };
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_arena_init(&a, buf, 0));
    TEST_ASSERT_EQUAL_size_t(0, mlg_arena_remaining(&a));
    TEST_ASSERT_NULL(mlg_arena_alloc(&a, 1, 1));
}

/* mark / rewind */

void test_arena_mark_on_fresh_arena_is_zero(void)
{
    mlg_arena_mark_t m = mlg_arena_mark(&arena);
    TEST_ASSERT_EQUAL_size_t(0, m.offset);
}

void test_arena_mark_captures_current_offset(void)
{
    mlg_arena_alloc(&arena, 3, 1);
    mlg_arena_alloc(&arena, 8, 8);
    mlg_arena_mark_t m = mlg_arena_mark(&arena);
    TEST_ASSERT_EQUAL_size_t(16, m.offset);
}

/* taking a mark is read-only */
void test_arena_mark_does_not_change_arena(void)
{
    mlg_arena_alloc(&arena, 10, 1);
    (void)mlg_arena_mark(&arena);
    TEST_ASSERT_EQUAL_size_t(10, arena.offset);
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE, arena.capacity);
    TEST_ASSERT_EQUAL_PTR(buf, arena.base);
}

void test_arena_rewind_restores_offset(void)
{
    mlg_arena_alloc(&arena, 10, 1);
    mlg_arena_mark_t m = mlg_arena_mark(&arena);

    mlg_arena_alloc(&arena, 20, 4);
    mlg_arena_alloc(&arena, 8, 8);
    mlg_arena_rewind(&arena, &m);

    TEST_ASSERT_EQUAL_size_t(10, arena.offset);
    TEST_ASSERT_EQUAL_size_t(10, mlg_arena_used(&arena));
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE - 10, mlg_arena_remaining(&arena));
}

/* the same alloc after a rewind lands at the same address */
void test_arena_alloc_after_rewind_reuses_memory(void)
{
    mlg_arena_alloc(&arena, 5, 1);
    mlg_arena_mark_t m = mlg_arena_mark(&arena);

    void *first = mlg_arena_alloc(&arena, 16, 8);
    mlg_arena_rewind(&arena, &m);
    void *again = mlg_arena_alloc(&arena, 16, 8);

    TEST_ASSERT_NOT_NULL(first);
    TEST_ASSERT_EQUAL_PTR(first, again);
}

/* allocations made before the mark survive the rewind */
void test_arena_rewind_keeps_earlier_allocations(void)
{
    unsigned char *keep = mlg_arena_alloc(&arena, 8, 1);
    memset(keep, 0xAA, 8);
    mlg_arena_mark_t m = mlg_arena_mark(&arena);

    unsigned char *tmp = mlg_arena_alloc(&arena, 16, 1);
    memset(tmp, 0xBB, 16);
    mlg_arena_rewind(&arena, &m);

    unsigned char *next = mlg_arena_alloc(&arena, 16, 1);
    memset(next, 0xCC, 16);

    TEST_ASSERT_EQUAL_PTR(buf + 8, next);
    assert_bytes(keep, 8, 0xAA);
}

/* rewind only moves the offset, it does not clear memory */
void test_arena_rewind_does_not_clear_memory(void)
{
    mlg_arena_mark_t m = mlg_arena_mark(&arena);
    unsigned char *p = mlg_arena_alloc(&arena, 16, 1);
    memset(p, DIRTY, 16);

    mlg_arena_rewind(&arena, &m);
    assert_bytes(buf, 16, DIRTY);

    /* alloc_zero is the way to get clean memory back */
    unsigned char *z = mlg_arena_alloc_zero(&arena, 16, 1);
    TEST_ASSERT_EQUAL_PTR(p, z);
    assert_bytes(z, 16, 0);
}

void test_arena_rewind_from_full_frees_space(void)
{
    mlg_arena_alloc(&arena, 24, 1);
    mlg_arena_mark_t m = mlg_arena_mark(&arena);

    TEST_ASSERT_NOT_NULL(mlg_arena_alloc(&arena, BUF_SIZE - 24, 1));
    TEST_ASSERT_NULL(mlg_arena_alloc(&arena, 1, 1));

    mlg_arena_rewind(&arena, &m);
    TEST_ASSERT_NOT_NULL(mlg_arena_alloc(&arena, BUF_SIZE - 24, 1));
}

/* rewind to a mark taken on an empty arena behaves like reset */
void test_arena_rewind_to_start_matches_reset(void)
{
    mlg_arena_mark_t m = mlg_arena_mark(&arena);
    mlg_arena_alloc(&arena, BUF_SIZE, 1);
    mlg_arena_rewind(&arena, &m);

    TEST_ASSERT_EQUAL_size_t(0, arena.offset);
    TEST_ASSERT_EQUAL_PTR(buf, mlg_arena_alloc(&arena, BUF_SIZE, 1));
}

/* rewinding with no allocations since the mark changes nothing */
void test_arena_rewind_to_current_offset_is_noop(void)
{
    mlg_arena_alloc(&arena, 12, 4);
    mlg_arena_mark_t m = mlg_arena_mark(&arena);
    mlg_arena_rewind(&arena, &m);
    TEST_ASSERT_EQUAL_size_t(12, arena.offset);
}

/* a mark can be reused for repeated scratch work, e.g. per loop iteration */
void test_arena_same_mark_can_be_rewound_repeatedly(void)
{
    mlg_arena_alloc(&arena, 4, 1);
    mlg_arena_mark_t m = mlg_arena_mark(&arena);

    for (int i = 0; i < 10; i++)
    {
        void *p = mlg_arena_alloc(&arena, 32, 8);
        TEST_ASSERT_EQUAL_PTR(buf + 8, p);
        mlg_arena_rewind(&arena, &m);
        TEST_ASSERT_EQUAL_size_t(4, arena.offset);
    }
}

/* rewind does not modify the mark itself */
void test_arena_rewind_leaves_mark_unchanged(void)
{
    mlg_arena_alloc(&arena, 6, 1);
    mlg_arena_mark_t m = mlg_arena_mark(&arena);
    mlg_arena_alloc(&arena, 20, 1);
    mlg_arena_rewind(&arena, &m);
    TEST_ASSERT_EQUAL_size_t(6, m.offset);
}

/* nested scopes unwind in LIFO order */
void test_arena_nested_marks_rewind_in_order(void)
{
    mlg_arena_alloc(&arena, 4, 1);
    mlg_arena_mark_t outer = mlg_arena_mark(&arena);

    mlg_arena_alloc(&arena, 8, 1);
    mlg_arena_mark_t inner = mlg_arena_mark(&arena);

    mlg_arena_alloc(&arena, 16, 1);
    TEST_ASSERT_EQUAL_size_t(28, arena.offset);

    mlg_arena_rewind(&arena, &inner);
    TEST_ASSERT_EQUAL_size_t(12, arena.offset);

    mlg_arena_rewind(&arena, &outer);
    TEST_ASSERT_EQUAL_size_t(4, arena.offset);
}

/* jumping straight to the outer mark discards everything after it */
void test_arena_rewind_to_outer_mark_skips_inner(void)
{
    mlg_arena_mark_t outer = mlg_arena_mark(&arena);
    mlg_arena_alloc(&arena, 8, 1);
    (void)mlg_arena_mark(&arena);
    mlg_arena_alloc(&arena, 8, 1);

    mlg_arena_rewind(&arena, &outer);
    TEST_ASSERT_EQUAL_size_t(0, arena.offset);
}

/* padding between the mark and the next alloc is given back too */
void test_arena_rewind_reclaims_padding(void)
{
    mlg_arena_alloc(&arena, 1, 1);
    mlg_arena_mark_t m = mlg_arena_mark(&arena);

    mlg_arena_alloc(&arena, 8, 16); /* 15 bytes of padding */
    TEST_ASSERT_EQUAL_size_t(24, arena.offset);

    mlg_arena_rewind(&arena, &m);
    TEST_ASSERT_EQUAL_size_t(1, arena.offset);
    TEST_ASSERT_EQUAL_PTR(buf + 1, mlg_arena_alloc(&arena, 1, 1));
}

/* a mark taken when the arena is full can be rewound to */
void test_arena_rewind_to_mark_at_capacity(void)
{
    mlg_arena_alloc(&arena, BUF_SIZE, 1);
    mlg_arena_mark_t m = mlg_arena_mark(&arena);
    mlg_arena_reset(&arena);

    mlg_arena_rewind(&arena, &m);
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE, arena.offset);
    TEST_ASSERT_EQUAL_size_t(0, mlg_arena_remaining(&arena));
}

/* mark / rewind: bad usage */

void test_arena_rewind_null_arena_is_ignored(void)
{
    mlg_arena_mark_t m = { 0 };
    mlg_arena_rewind(NULL, &m); /* must not crash */
    TEST_ASSERT_EQUAL_size_t(0, m.offset);
}

void test_arena_rewind_null_mark_is_ignored(void)
{
    mlg_arena_alloc(&arena, 10, 1);
    mlg_arena_rewind(&arena, NULL);
    TEST_ASSERT_EQUAL_size_t(10, arena.offset);
}

void test_arena_rewind_both_null_is_ignored(void)
{
    mlg_arena_rewind(NULL, NULL); /* must not crash */
    TEST_PASS();
}

/* a mark past the end must not let the offset escape the buffer */
void test_arena_rewind_out_of_bounds_mark_is_ignored(void)
{
    mlg_arena_alloc(&arena, 10, 1);

    mlg_arena_mark_t m = { .offset = BUF_SIZE + 1 };
    mlg_arena_rewind(&arena, &m);
    TEST_ASSERT_EQUAL_size_t(10, arena.offset);

    m.offset = SIZE_MAX;
    mlg_arena_rewind(&arena, &m);
    TEST_ASSERT_EQUAL_size_t(10, arena.offset);
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE - 10, mlg_arena_remaining(&arena));
}

/* the arena must stay fully usable after a rejected rewind */
void test_arena_alloc_works_after_rejected_rewind(void)
{
    mlg_arena_mark_t m = { .offset = BUF_SIZE * 2 };
    mlg_arena_rewind(&arena, &m);

    TEST_ASSERT_EQUAL_PTR(buf, mlg_arena_alloc(&arena, BUF_SIZE, 1));
    TEST_ASSERT_NULL(mlg_arena_alloc(&arena, 1, 1));
}

/* a mark from a bigger arena can't push a smaller one out of bounds */
void test_arena_rewind_with_mark_from_larger_arena_is_ignored(void)
{
    static unsigned char big_buf[BUF_SIZE * 2];
    mlg_arena_t big;
    mlg_arena_init(&big, big_buf, sizeof big_buf);
    mlg_arena_alloc(&big, BUF_SIZE + 8, 1);
    mlg_arena_mark_t m = mlg_arena_mark(&big);

    mlg_arena_alloc(&arena, 4, 1);
    mlg_arena_rewind(&arena, &m);
    TEST_ASSERT_EQUAL_size_t(4, arena.offset);
}

/* rewind cannot rescue an arena that was never set up */
void test_arena_rewind_on_zeroed_arena(void)
{
    mlg_arena_t empty = { 0 };
    mlg_arena_mark_t m = { .offset = 1 };

    mlg_arena_rewind(&empty, &m); /* capacity 0: out of bounds */
    TEST_ASSERT_EQUAL_size_t(0, empty.offset);
    TEST_ASSERT_NULL(mlg_arena_alloc(&empty, 1, 1));
}

/*
 * Rewinding "forward" to a mark ahead of the current offset is not detected:
 * the offset is only bounds-checked. These tests pin down that behavior so a
 * change to it is a conscious decision.
 */

/* stale mark after reset: the skipped bytes are simply lost */
void test_arena_rewind_forward_after_reset_is_accepted(void)
{
    mlg_arena_alloc(&arena, 32, 1);
    mlg_arena_mark_t m = mlg_arena_mark(&arena);
    mlg_arena_reset(&arena);

    mlg_arena_rewind(&arena, &m);
    TEST_ASSERT_EQUAL_size_t(32, arena.offset);
    TEST_ASSERT_EQUAL_size_t(BUF_SIZE - 32, mlg_arena_remaining(&arena));
}

/* rewinding the inner mark after the outer one moves the offset forward again */
void test_arena_rewind_out_of_order_moves_forward(void)
{
    mlg_arena_mark_t outer = mlg_arena_mark(&arena);
    mlg_arena_alloc(&arena, 8, 1);
    mlg_arena_mark_t inner = mlg_arena_mark(&arena);
    mlg_arena_alloc(&arena, 8, 1);

    mlg_arena_rewind(&arena, &outer);
    mlg_arena_rewind(&arena, &inner);
    TEST_ASSERT_EQUAL_size_t(8, arena.offset);
}

/*
 * Stale mark misuse: rewind to an old mark, allocate past it, then rewind to
 * a later stale mark. The next alloc hands out memory that is still live.
 * The arena can't detect this; callers must rewind marks in LIFO order.
 */
void test_arena_stale_mark_can_cause_overlapping_allocs(void)
{
    mlg_arena_mark_t start = mlg_arena_mark(&arena);
    mlg_arena_alloc(&arena, 8, 1);
    mlg_arena_mark_t stale = mlg_arena_mark(&arena);

    mlg_arena_rewind(&arena, &start);
    unsigned char *live = mlg_arena_alloc(&arena, 16, 1); /* bytes 0..16 */

    mlg_arena_rewind(&arena, &stale);                     /* offset 8 */
    unsigned char *overlap = mlg_arena_alloc(&arena, 8, 1);

    TEST_ASSERT_EQUAL_PTR(buf, live);
    TEST_ASSERT_EQUAL_PTR(buf + 8, overlap);
    TEST_ASSERT_TRUE(overlap < live + 16); /* overlaps the live block */
}

int main(void)
{
    UnityBegin("test_allocators_arena.c");

    /* init */
    RUN_TEST(test_arena_init_sets_fields);
    RUN_TEST(test_arena_init_rejects_invalid_args);
    /* basic allocation */
    RUN_TEST(test_arena_first_alloc_returns_base);
    RUN_TEST(test_arena_consecutive_allocs_bump_offset);
    RUN_TEST(test_arena_padding_walkthrough);
    RUN_TEST(test_arena_typed_allocs_are_aligned);
    RUN_TEST(test_arena_every_power_of_two_alignment);
    RUN_TEST(test_arena_unaligned_base_still_returns_aligned_pointers);
    RUN_TEST(test_arena_allocations_do_not_overlap);
    /* capacity limits */
    RUN_TEST(test_arena_exact_fit_succeeds);
    RUN_TEST(test_arena_alloc_when_full_fails);
    RUN_TEST(test_arena_too_large_fails_and_leaves_offset);
    RUN_TEST(test_arena_padding_pushes_alloc_over_capacity);
    RUN_TEST(test_arena_huge_size_fails);
    RUN_TEST(test_arena_fill_with_single_bytes);
    /* invalid arguments */
    RUN_TEST(test_arena_alloc_rejects_null_arena);
    RUN_TEST(test_arena_alloc_zero_size_returns_aligned_pointer);
    RUN_TEST(test_arena_alloc_zero_size_only_consumes_padding);
    RUN_TEST(test_arena_alloc_zero_size_succeeds_when_full);
    RUN_TEST(test_arena_alloc_rejects_non_power_of_two_alignment);
    /* reset */
    RUN_TEST(test_arena_reset_rewinds_offset_and_keeps_capacity);
    RUN_TEST(test_arena_alloc_after_reset_reuses_memory);
    RUN_TEST(test_arena_full_capacity_available_after_reset);
    RUN_TEST(test_arena_reset_rejects_null);
    /* alloc_zero */
    RUN_TEST(test_arena_alloc_zero_clears_reused_memory);
    RUN_TEST(test_arena_alloc_zero_clears_whole_capacity);
    RUN_TEST(test_arena_alloc_zero_only_touches_its_own_bytes);
    RUN_TEST(test_arena_alloc_zero_struct_fields_start_empty);
    RUN_TEST(test_arena_alloc_zero_matches_alloc_layout);
    RUN_TEST(test_arena_alloc_zero_failure_leaves_buffer_untouched);
    RUN_TEST(test_arena_alloc_zero_huge_size_fails);
    RUN_TEST(test_arena_alloc_zero_rejects_invalid_args);

    RUN_TEST(test_arena_used_and_remaining_on_fresh_arena);
    RUN_TEST(test_arena_used_and_remaining_track_allocs);
    RUN_TEST(test_arena_used_includes_padding);
    RUN_TEST(test_arena_used_plus_remaining_is_capacity);
    RUN_TEST(test_arena_used_and_remaining_when_full);
    RUN_TEST(test_arena_failed_alloc_does_not_change_used);
    RUN_TEST(test_arena_reset_restores_used_and_remaining);
    RUN_TEST(test_arena_used_rejects_null);
    RUN_TEST(test_arena_remaining_rejects_null);
    RUN_TEST(test_arena_remaining_does_not_underflow_on_corrupt_offset);
    RUN_TEST(test_arena_used_and_remaining_on_zeroed_struct);
    RUN_TEST(test_arena_failed_init_leaves_arena_untouched);
    /* mark / rewind */
    RUN_TEST(test_arena_mark_on_fresh_arena_is_zero);
    RUN_TEST(test_arena_mark_captures_current_offset);
    RUN_TEST(test_arena_mark_does_not_change_arena);
    RUN_TEST(test_arena_rewind_restores_offset);
    RUN_TEST(test_arena_alloc_after_rewind_reuses_memory);
    RUN_TEST(test_arena_rewind_keeps_earlier_allocations);
    RUN_TEST(test_arena_rewind_does_not_clear_memory);
    RUN_TEST(test_arena_rewind_from_full_frees_space);
    RUN_TEST(test_arena_rewind_to_start_matches_reset);
    RUN_TEST(test_arena_rewind_to_current_offset_is_noop);
    RUN_TEST(test_arena_same_mark_can_be_rewound_repeatedly);
    RUN_TEST(test_arena_rewind_leaves_mark_unchanged);
    RUN_TEST(test_arena_nested_marks_rewind_in_order);
    RUN_TEST(test_arena_rewind_to_outer_mark_skips_inner);
    RUN_TEST(test_arena_rewind_reclaims_padding);
    RUN_TEST(test_arena_rewind_to_mark_at_capacity);
    RUN_TEST(test_arena_rewind_null_arena_is_ignored);
    RUN_TEST(test_arena_rewind_null_mark_is_ignored);
    RUN_TEST(test_arena_rewind_both_null_is_ignored);
    RUN_TEST(test_arena_rewind_out_of_bounds_mark_is_ignored);
    RUN_TEST(test_arena_alloc_works_after_rejected_rewind);
    RUN_TEST(test_arena_rewind_with_mark_from_larger_arena_is_ignored);
    RUN_TEST(test_arena_rewind_on_zeroed_arena);
    RUN_TEST(test_arena_rewind_forward_after_reset_is_accepted);
    RUN_TEST(test_arena_rewind_out_of_order_moves_forward);
    RUN_TEST(test_arena_stale_mark_can_cause_overlapping_allocs);

    return UnityEnd();
}
