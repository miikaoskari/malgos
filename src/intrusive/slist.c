#include <stddef.h>

#include "malgos/intrusive/slist.h"
#include "malgos/common/types.h"

mlg_error_t mlg_slist_init(mlg_slist_t *slist)
{
    if (!slist)
    {
        return MLG_ERROR;
    }

    slist->head.next = &slist->head;
    slist->tail = &slist->head;

    return MLG_OK;
}

/**
 * @brief Insertion helper function
 *
 * @param *new_node
 * @param *next
 */
static inline void __mlg_slist_insert(mlg_slist_node_t *new_node, mlg_slist_node_t *next)
{
    new_node->next = next->next;
    next->next = new_node;
}

mlg_error_t mlg_slist_push_back(mlg_slist_t *slist, mlg_slist_node_t *node)
{
    if (!slist || !node)
    {
        return MLG_ERROR;
    }

    __mlg_slist_insert(node, slist->tail);
    slist->tail = node;

    return MLG_OK;
}

mlg_error_t mlg_slist_push_front(mlg_slist_t *slist, mlg_slist_node_t *node)
{
    if (!slist || !node)
    {
        return MLG_ERROR;
    }

    __mlg_slist_insert(node, &slist->head);

    if (slist->tail == &slist->head)
    {
        slist->tail = node;
    }

    return MLG_OK;
}

mlg_error_t mlg_slist_remove(mlg_slist_t *slist, mlg_slist_node_t *node)
{
    if (!slist || !node)
    {
        return MLG_ERROR;
    }

    mlg_slist_node_t *prev = &slist->head;
    mlg_slist_node_t *pos;

    mlg_slist_for_each(pos, slist)
    {
        if (pos == node)
        {
            prev->next = node->next;

            if (slist->tail == node)
            {
                slist->tail = prev;
            }

            node->next = NULL;

            return MLG_OK;
        }

        prev = pos;
    }

    return MLG_ERROR;
}

mlg_error_t mlg_slist_insert_after(mlg_slist_t *slist, mlg_slist_node_t *node, mlg_slist_node_t *after)
{
    if (!slist || !node || !after || !after->next)
    {
        return MLG_ERROR;
    }

    __mlg_slist_insert(node, after);

    if (slist->tail == after)
    {
        slist->tail = node;
    }

    return MLG_OK;
}

mlg_error_t mlg_slist_insert_before(mlg_slist_t *slist, mlg_slist_node_t *node, mlg_slist_node_t *before)
{
    if (!slist || !node || !before)
    {
        return MLG_ERROR;
    }

    mlg_slist_node_t *prev = &slist->head;
    while (prev->next != before)
    {
        if (prev->next == &slist->head)
        {
            return MLG_ERROR;
        }

        prev = prev->next;
    }

    __mlg_slist_insert(node, prev);

    if (before == &slist->head)
    {
        slist->tail = node;
    }

    return MLG_OK;
}
