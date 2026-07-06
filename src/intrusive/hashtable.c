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
    memset(hash_table->buckets, 0, size * sizeof(*hash_table->buckets));
    return MLG_OK;
}

mlg_error_t mlg_hashtable_insert(mlg_hash_table_t *hash_table, mlg_hash_node_t *node, size_t key)
{
    if (!hash_table || !hash_table->buckets || !node || hash_table->size == 0)
    {
        return MLG_ERROR;
    }

    size_t idx = key % hash_table->size;
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

mlg_error_t mlg_hashtable_rehash(mlg_hash_table_t *old_table, mlg_hash_table_t *new_table, mlg_hash_key_cb key_cb)
{
    if (!old_table || !new_table || !key_cb)
    {
        return MLG_ERROR;
    }

    mlg_hash_node_t *pos;
    size_t bkt = 0;
    mlg_hash_node_t *n;
    mlg_hash_for_each_safe(pos, old_table, bkt, n)
    {
        size_t key = key_cb(pos);
        mlg_hashtable_remove(pos);
        mlg_hashtable_insert(new_table, pos, key);
    }

    return MLG_OK;
}
