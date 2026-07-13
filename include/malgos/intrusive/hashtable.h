#ifndef MLG_INTRUSIVE_HASHTABLE_H
#define MLG_INTRUSIVE_HASHTABLE_H

#include <stddef.h>
#include <stdint.h>

#include "malgos/common/types.h"

#define mlg_hash_for_each_possible(pos, hash_table, hash)                                                              \
    for ((pos) = (hash_table)->buckets[(hash) % (hash_table)->size].first; (pos) != NULL; (pos) = (pos)->next)

#define mlg_hash_for_each(pos, hash_table, bkt)                                                                        \
    for ((bkt) = 0; (bkt) < (hash_table)->size; (bkt)++)                                                               \
        for ((pos) = (hash_table)->buckets[bkt].first; (pos) != NULL; (pos) = (pos)->next)

#define mlg_hash_for_each_safe(pos, hash_table, bkt, n)                                                                \
    for ((bkt) = 0; (bkt) < (hash_table)->size; (bkt)++)                                                               \
        for ((pos) = (hash_table)->buckets[bkt].first; (pos) != NULL && ((n) = (pos)->next, 1); (pos) = (n))

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
mlg_error_t mlg_hashtable_insert(mlg_hash_table_t *hash_table, mlg_hash_node_t *node, size_t key);
mlg_error_t mlg_hashtable_remove(mlg_hash_node_t *node);
mlg_error_t mlg_hashtable_rehash(mlg_hash_table_t *old_table, mlg_hash_table_t *new_table, mlg_hash_key_cb key_cb);

#endif // MLG_INTRUSIVE_HASHTABLE_H
