#ifndef MLG_INTRUSIVE_HASHTABLE_H
#define MLG_INTRUSIVE_HASHTABLE_H

#include <stdint.h>
#include <stddef.h>

#include "malgos/common/types.h"

struct mlg_hash_node_s {
    struct mlg_hash_node_s *next;
    struct mlg_hash_node_s *prev;
    uint32_t hash;
};

struct mlg_hash_table_s {
    struct mlg_hash_node_s **buckets;
    size_t size;
};

mlg_error_t mlg_hashtable_init(struct mlg_hash_table_s *hash_table, struct mlg_hash_node_s **buckets, size_t size);
mlg_error_t mlg_hashtable_insert(struct mlg_hash_table_s *hash_table, struct mlg_hash_node_s *node);
mlg_error_t mlg_hashtable_remove(struct mlg_hash_table_s *hash_table, struct mlg_hash_node_s *node);
struct mlg_hash_node_s* mlg_hashtable_get(struct mlg_hash_table_s *hash_table, uint32_t hash);

#endif // MLG_INTRUSIVE_HASHTABLE_H
