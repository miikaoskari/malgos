#ifndef MLG_INTRUSIVE_HASHTABLE_H
#define MLG_INTRUSIVE_HASHTABLE_H

#include <stddef.h>
#include <stdint.h>

#include "malgos/common/types.h"

#define mlg_hash_for_each_possible(pos, hash_table, hash) \
    for ((pos) = (hash_table)->buckets[(hash) % (hash_table)->size]; (pos) != NULL; (pos) = (pos)->next)

typedef struct mlg_hash_node_s mlg_hash_node_t;
typedef struct mlg_hash_table_s mlg_hash_table_t;

struct mlg_hash_node_s
{
    mlg_hash_node_t *next;
    mlg_hash_node_t *prev;
    uint32_t hash;
};

struct mlg_hash_table_s
{
    mlg_hash_node_t **buckets;
    size_t size;
};

mlg_error_t mlg_hashtable_init(mlg_hash_table_t *hash_table, mlg_hash_node_t **buckets, size_t size);
mlg_error_t mlg_hashtable_insert(mlg_hash_table_t *hash_table, mlg_hash_node_t *node);
mlg_error_t mlg_hashtable_remove(mlg_hash_table_t *hash_table, mlg_hash_node_t *node);

#endif // MLG_INTRUSIVE_HASHTABLE_H
