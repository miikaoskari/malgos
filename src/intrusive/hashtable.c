#include "malgos/intrusive/hashtable.h"
#include "malgos/common/types.h"

#define is_power_of_2(x) ((x) != 0 && (((x) & ((x) - 1)) == 0))

static inline mlg_error_t mlg_is_table_valid(mlg_hash_table_t *hash_table)
{
    if(!hash_table || !hash_table->buckets || !hash_table->size)
    {
        return MLG_ERROR;
    }

    return MLG_OK;
}

mlg_error_t mlg_hashtable_init(mlg_hash_table_t *hash_table, mlg_hash_head_t *buckets, size_t size)
{
    if (!hash_table || size == 0 || !buckets || !is_power_of_2(size))
    {
        return MLG_ERROR;
    }

    hash_table->size = size;
    hash_table->buckets = buckets;
    for (size_t i = 0; i < size; i++)
    {
        hash_table->buckets[i].first = NULL;
    }
    return MLG_OK;
}

mlg_error_t mlg_hashtable_node_init(mlg_hash_node_t *node)
{
    if (!node)
    {
        return MLG_ERROR;
    }

    node->next = NULL;
    node->pprev = NULL;

    return MLG_OK;
}

mlg_error_t mlg_hashtable_insert(mlg_hash_table_t *hash_table, mlg_hash_node_t *node, size_t key)
{
    if (mlg_is_table_valid(hash_table) || !node)
    {
        return MLG_ERROR;
    }

    /* re-inserting an already linked node corrupts the chain it is already on */
    if (node->pprev)
    {
        return MLG_ERROR;
    }

    size_t idx = key & (hash_table->size - 1);
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
    if (mlg_is_table_valid(old_table) || mlg_is_table_valid(new_table) || !key_cb || (old_table == new_table))
    {
        return MLG_ERROR;
    }

    mlg_hash_node_t *pos;
    size_t bkt = 0;
    mlg_hash_node_t *n;
    mlg_hash_for_each_node_safe(old_table, bkt, pos, n)
    {
        size_t key = key_cb(pos);
        mlg_hashtable_remove(pos);
        mlg_hashtable_insert(new_table, pos, key);
    }

    return MLG_OK;
}
