#include <stddef.h>
#include <string.h>

#include "malgos/common/types.h"
#include "malgos/intrusive/hashtable.h"

mlg_error_t mlg_hashtable_init(struct mlg_hash_table_s *hash_table, struct mlg_hash_node_s **buckets, size_t size)
{
    if (!hash_table || size == 0 || !buckets)
        return MLG_ERROR;

    hash_table->size = size;
    hash_table->buckets = buckets;
    memset(hash_table->buckets, 0, size * sizeof(struct mlg_hash_node_s *));
}

mlg_error_t mlg_hashtable_insert(struct mlg_hash_table_s *hash_table, struct mlg_hash_node_s *node)
{
    if (!hash_table || !hash_table->buckets || !node || hash_table->size == 0)
    {
        return MLG_ERROR;
    }

    uint32_t idx = node->hash % hash_table->size;
    struct mlg_hash_node_s *next = hash_table->buckets[idx];

    node->next = next;
    node->prev = NULL;

    if (next)
    {
        next->prev = node;
    }
    hash_table->buckets[idx] = node;

    return MLG_OK;
}

mlg_error_t mlg_hashtable_remove(struct mlg_hash_table_s *hash_table, struct mlg_hash_node_s *node)
{
    if (!hash_table || !hash_table->buckets || !node || hash_table->size == 0)
    {
        return MLG_ERROR;
    }

    if (node->prev)
    {
        node->prev->next = node->next;
    }
    else
    {
        uint32_t idx = node->hash % hash_table->size;
        hash_table->buckets[idx] = node->next;
    }

    if (node->next)
    {
        node->next->prev = node->prev;
    }

    node->next = NULL;
    node->prev = NULL;

    return MLG_OK;
}

struct mlg_hash_node_s *mlg_hashtable_get(struct mlg_hash_table_s *hash_table, uint32_t hash)
{
    if (!hash_table || !hash_table->buckets || hash_table->size == 0)
    {
        return NULL;
    }

    size_t idx = hash % hash_table->size;
    struct mlg_hash_node_s *current = hash_table->buckets[idx];

    while (current)
    {
        // TODO: needs a callback comparison function to handle collisions
        if (current->hash == hash)
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}
