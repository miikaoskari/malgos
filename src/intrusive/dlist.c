#include <stddef.h>

#include "malgos/intrusive/dlist.h"
#include "malgos/common/types.h"

mlg_error_t mlg_dlist_init(mlg_dlist_t *dlist)
{
    if (!dlist)
    {
        return MLG_ERROR;
    }

    dlist->head.next = &dlist->head;
    dlist->head.prev = &dlist->head;

    return MLG_OK;
}

static inline void __mlg_dlist_insert(mlg_dlist_node_t *new_node, mlg_dlist_node_t *prev, mlg_dlist_node_t *next)
{
    next->prev = new_node;
    new_node->next = next;
    new_node->prev = prev;
    prev->next = new_node;
}

mlg_error_t mlg_dlist_push_back(mlg_dlist_t *dlist, mlg_dlist_node_t *node)
{
    if (!dlist || !node)
    {
        return MLG_ERROR;
    }

    __mlg_dlist_insert(node, &dlist->head, &dlist->head);

    return MLG_OK;
}

mlg_error_t mlg_dlist_push_front(mlg_dlist_t *dlist, mlg_dlist_node_t *node)
{
    if (!dlist || !node)
    {
        return MLG_ERROR;
    }

    __mlg_dlist_insert(node, dlist->head.prev, &dlist->head);

    return MLG_OK;
}

mlg_error_t mlg_dlist_remove(mlg_dlist_node_t *node)
{
    if (!node || !node->prev || !node->next)
    {
        return MLG_ERROR;
    }

    node->next->prev = node->prev;
    node->prev->next = node->next;

    node->next = NULL;
    node->prev = NULL;

    return MLG_OK;
}
