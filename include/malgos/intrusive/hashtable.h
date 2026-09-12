#ifndef MLG_INTRUSIVE_HASHTABLE_H
#define MLG_INTRUSIVE_HASHTABLE_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

#include "malgos/common/types.h"
#include "malgos/common/utils.h"

/* clang-format off */

/**
 * @brief static initializer for an mlg_hash_node_t embedded in an entry.
 *
 * Equivalent to mlg_hashtable_node_init(), but usable in a struct initializer
 */
#define mlg_hash_node_init { .next = NULL, .pprev = NULL }

/**
 * @brief get the entry containing a hash node, or NULL if the node pointer is NULL.
 *
 * @param ptr pointer to an mlg_hash_node_t, or NULL.
 * @param type type of the containing struct.
 * @param member name of the mlg_hash_node_t member within @p type.
 */
#define mlg_hash_entry_safe(ptr, type, member) mlg_container_of_safe(ptr, type, member)


/**
 * @brief iterate over the typed entries in a single bucket.
 *
 * @param head pointer to the mlg_hash_head_t bucket to scan.
 * @param pos cursor: a pointer to the entry type containing @p member.
 * @param member name of the mlg_hash_node_t member within the entry type.
 *
 * @note not safe against removing @p pos during iteration
 */
#define mlg_hash_for_each_entry(head, pos, member) \
    __mlg_hash_for_each_entry(head, pos, member, mlg_unique_name(__mlg_head_))

#define __mlg_hash_for_each_entry(head, pos, member, h)                     \
    for (mlg_hash_head_t *h = (head); h; h = NULL)                          \
        for ((pos) = mlg_hash_entry_safe(h->first, typeof(*(pos)), member); \
             (pos);                                                         \
             (pos) = mlg_hash_entry_safe((pos)->member.next, typeof(*(pos)), member))

/**
 * @brief iterate over the typed entries in a single bucket, safe against removing @p pos.
 *
 * @param head pointer to the mlg_hash_head_t bucket to scan.
 * @param pos cursor: a pointer to the entry type containing @p member.
 * @param n mlg_hash_node_t* that stashes the next node before the body runs, so @p pos may be removed
 * @param member name of the mlg_hash_node_t member within the entry type.
 */
#define mlg_hash_for_each_entry_safe(head, pos, n, member) \
    __mlg_hash_for_each_entry_safe(head, pos, n, member, mlg_unique_name(__mlg_head_))

#define __mlg_hash_for_each_entry_safe(head, pos, n, member, h)             \
    for (mlg_hash_head_t *h = (head); h; h = NULL)                          \
        for ((pos) = mlg_hash_entry_safe(h->first, typeof(*(pos)), member); \
             (pos) && ({ (n) = (pos)->member.next; 1; });                   \
             (pos) = mlg_hash_entry_safe((n), typeof(*(pos)), member))


/**
 * @brief iterate over the raw nodes in a single bucket, safe against removing @p pos.
 *
 * @param head pointer to the mlg_hash_head_t bucket to scan.
 * @param pos cursor of type mlg_hash_node_t*.
 * @param n mlg_hash_node_t* that stashes the next node before the body runs, so @p pos may be removed
 *
 * @note low-level primitive that yields raw nodes rather than typed entries that has no entry type to attach @p member to.
 */
#define mlg_hash_bucket_for_each_node_safe(head, pos, n) \
    for ((pos) = (head)->first;                          \
         (pos) && ((n) = (pos)->next, 1);                \
         (pos) = (n))

/**
 * @brief iterate over the raw nodes of every bucket in a table, safe against removing @p pos.
 *
 * @param hash_table pointer to the mlg_hash_table_t to iterate.
 * @param bkt size_t scratch variable tracking the current bucket index.
 * @param pos cursor of type mlg_hash_node_t*.
 * @param n mlg_hash_node_t* that stashes the next node before the body runs, so @p pos may be removed.
 *
 * @note low-level primitive that yields raw nodes rather than typed entries that has no entry type to attach @p member to.
 */
#define mlg_hash_for_each_node_safe(hash_table, bkt, pos, n) \
    __mlg_hash_for_each_node_safe(hash_table, bkt, pos, n, mlg_unique_name(__mlg_tbl_))

#define __mlg_hash_for_each_node_safe(hash_table, bkt, pos, n, t) \
    for (mlg_hash_table_t *t = (hash_table); t; t = NULL)         \
        for ((bkt) = 0, (pos) = NULL;                             \
             (pos) == NULL && t->buckets && (bkt) < t->size;      \
             (bkt)++)                                             \
            mlg_hash_bucket_for_each_node_safe(&t->buckets[bkt], pos, n)


/**
 * @brief iterate over the typed entries that could match @p hash, i.e. those in its bucket.
 *
 * @param hash_table pointer to the mlg_hash_table_t to look up in.
 * @param pos cursor: a pointer to the entry type containing @p member.
 * @param member name of the mlg_hash_node_t member within the entry type.
 * @param hash key hash selecting the bucket to scan.
 *
 * @note not safe against removing @p pos during iteration
 */
#define mlg_hash_for_each_possible(hash_table, pos, member, hash) \
    __mlg_hash_for_each_possible(hash_table, pos, member, hash, mlg_unique_name(__mlg_ht_))

#define __mlg_hash_for_each_possible(hash_table, pos, member, hash, t)  \
    mlg_hash_for_each_entry(                                            \
        ({ mlg_hash_table_t *t = (hash_table);                          \
           (t && t->buckets && t->size) ?                               \
               &t->buckets[(hash) & (t->size - 1)] : NULL; }),          \
        pos, member)

/**
 * @brief iterate over the typed entries that could match @p hash, safe against removing @p pos.
 *
 * @param hash_table pointer to the mlg_hash_table_t to look up in.
 * @param pos cursor: a pointer to the entry type containing @p member.
 * @param n mlg_hash_node_t* that stashes the next node before the body runs, so @p pos may be removed.
 * @param member name of the mlg_hash_node_t member within the entry type.
 * @param hash key hash selecting the bucket to scan.
 */
#define mlg_hash_for_each_possible_safe(hash_table, pos, n, member, hash) \
    __mlg_hash_for_each_possible_safe(hash_table, pos, n, member, hash, mlg_unique_name(__mlg_ht_))

#define __mlg_hash_for_each_possible_safe(hash_table, pos, n, member, hash, t) \
    mlg_hash_for_each_entry_safe(                                              \
        ({ mlg_hash_table_t *t = (hash_table);                                 \
           (t && t->buckets && t->size) ?                                      \
               &t->buckets[(hash) & (t->size - 1)] : NULL; }),                 \
        pos, n, member)


/**
 * @brief iterate over the typed entries of every bucket in a table.
 *
 * @param hash_table pointer to the mlg_hash_table_t to iterate.
 * @param bkt size_t scratch variable tracking the current bucket index.
 * @param pos cursor: a pointer to the entry type containing @p member.
 * @param member name of the mlg_hash_node_t member within the entry type.
 *
 * @note not safe against removing @p pos during iteration
 */
#define mlg_hash_for_each(hash_table, bkt, pos, member) \
    __mlg_hash_for_each(hash_table, bkt, pos, member, mlg_unique_name(__mlg_tbl_))

#define __mlg_hash_for_each(hash_table, bkt, pos, member, t)  \
    for (mlg_hash_table_t *t = (hash_table); t; t = NULL)     \
        for ((bkt) = 0, (pos) = NULL;                         \
             (pos) == NULL && t->buckets && (bkt) < t->size;  \
             (bkt)++)                                         \
            mlg_hash_for_each_entry(&t->buckets[bkt], pos, member)

/**
 * @brief iterate over the typed entries of every bucket in a table, safe against removing @p pos.
 *
 * @param hash_table pointer to the mlg_hash_table_t to iterate.
 * @param bkt size_t scratch variable tracking the current bucket index.
 * @param pos cursor: a pointer to the entry type containing @p member.
 * @param n mlg_hash_node_t* that stashes the next node before the body runs, so @p pos may be removed or freed in the body.
 * @param member name of the mlg_hash_node_t member within the entry type.
 */
#define mlg_hash_for_each_safe(hash_table, bkt, pos, n, member) \
    __mlg_hash_for_each_safe(hash_table, bkt, pos, n, member, mlg_unique_name(__mlg_tbl_))

#define __mlg_hash_for_each_safe(hash_table, bkt, pos, n, member, t) \
    for (mlg_hash_table_t *t = (hash_table); t; t = NULL)            \
        for ((bkt) = 0, (pos) = NULL;                                \
             (pos) == NULL && t->buckets && (bkt) < t->size;         \
             (bkt)++)                                                \
            mlg_hash_for_each_entry_safe(&t->buckets[bkt], pos, n, member)

/* clang-format on */

typedef struct mlg_hash_head_s mlg_hash_head_t;
typedef struct mlg_hash_node_s mlg_hash_node_t;
typedef struct mlg_hash_table_s mlg_hash_table_t;
typedef size_t (*mlg_hash_key_cb)(mlg_hash_node_t *node);

struct mlg_hash_head_s
{
    mlg_hash_node_t *first;
};

struct mlg_hash_node_s
{
    mlg_hash_node_t *next;
    mlg_hash_node_t **pprev;
};

struct mlg_hash_table_s
{
    mlg_hash_head_t *buckets;
    size_t size;
};

mlg_error_t mlg_hashtable_init(mlg_hash_table_t *hash_table, mlg_hash_head_t *buckets, size_t size);

/**
 * @brief put a node into the unlinked state, so it can be inserted and safely queried.
 *
 * Every node must be initialized -- with this call or with MLG_HASH_NODE_INIT -- before it is
 * first passed to mlg_hashtable_insert() or mlg_hashtable_remove(). Both of those distinguish a
 * linked node from an unlinked one by reading @c pprev, so an uninitialized node makes them act
 * on an indeterminate pointer.
 *
 * @param node node to initialize.
 * @return MLG_OK, or MLG_ERROR if @p node is NULL.
 */
mlg_error_t mlg_hashtable_node_init(mlg_hash_node_t *node);

/**
 * @brief link an initialized, unlinked node into the bucket that @p key hashes to.
 *
 * @param hash_table table to insert into.
 * @param node initialized node that is not currently in any table.
 * @param key key whose low bits select the bucket.
 * @return MLG_OK, or MLG_ERROR if the table is invalid, @p node is NULL, or @p node is already
 *         linked into a table.
 */
mlg_error_t mlg_hashtable_insert(mlg_hash_table_t *hash_table, mlg_hash_node_t *node, size_t key);

/**
 * @brief unlink a node from whichever table holds it, leaving it in the unlinked state.
 *
 * @param node initialized node.
 * @return MLG_OK, or MLG_ERROR if @p node is NULL or not currently linked.
 */
mlg_error_t mlg_hashtable_remove(mlg_hash_node_t *node);
mlg_error_t mlg_hashtable_rehash(mlg_hash_table_t *old_table, mlg_hash_table_t *new_table, mlg_hash_key_cb key_cb);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLG_INTRUSIVE_HASHTABLE_H */
