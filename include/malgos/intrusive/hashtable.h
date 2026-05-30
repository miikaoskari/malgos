#ifndef MLG_INTRUSIVE_HASHTABLE_H
#define MLG_INTRUSIVE_HASHTABLE_H

#include <stddef.h>
#include <stdint.h>

#include "malgos/common/types.h"

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
mlg_hash_node_t *mlg_hashtable_get(mlg_hash_table_t *hash_table, uint32_t hash);

#endif // MLG_INTRUSIVE_HASHTABLE_H
