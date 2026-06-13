#include <stddef.h>
#include <string.h>

#include "malgos/common/types.h"
#include "malgos/intrusive/hashtable.h"


mlg_error_t mlg_hashtable_init(mlg_hash_table_t *hash_table, mlg_hash_head_t *buckets, size_t size)
{
    if (!hash_table || size == 0 || !buckets)
    {
        return MLG_ERROR;
    }

    hash_table->size = size;
    hash_table->buckets = buckets;
    memset(hash_table->buckets, 0, size * sizeof(mlg_hash_head_t *));
    return MLG_OK;
}

mlg_error_t mlg_hashtable_insert(mlg_hash_table_t *hash_table, mlg_hash_node_t *node)
{
    if (!hash_table || !hash_table->buckets || !node || hash_table->size == 0)
    {
        return MLG_ERROR;
    }

    size_t idx = node->hash % hash_table->size;
    mlg_hash_head_t *head = &hash_table->buckets[idx];

    mlg_hash_node_t *first = head->first;

    node->next = first;
    if (first)
    {
        first->pprev = &node->next;
    }

    head->first = node;
    node->pprev = &head->first;

    return MLG_OK;
}

mlg_error_t mlg_hashtable_remove(mlg_hash_node_t *node)
{
    if (!node || !node->pprev)
    {
        return MLG_ERROR;
    }

    *(node->pprev) = node->next;

    if (node->next)
    {
        node->next->pprev = node->pprev;
    }

    node->next = NULL;
    node->pprev = NULL;

    return MLG_OK;
}
