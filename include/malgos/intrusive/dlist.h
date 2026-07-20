#ifndef MLG_DLIST_H
#define MLG_DLIST_H

#include "malgos/common/types.h"

#define mlg_dlist_for_each(pos, dlist) for ((pos) = (dlist)->head.next; (pos) != &(list)->head; (pos) = (pos)->next)

#define mlg_dlist_for_each_safe(pos, n, list)                                                                          \
    for ((pos) = (list)->head.next, (n) = (pos)->next; (pos) != &(list)->head; (pos) = (n), (n) = (pos)->next)

typedef struct mlg_dlist_node_s mlg_dlist_node_t;
typedef struct mlg_dlist_s mlg_dlist_t;

struct mlg_dlist_node_s
{
    mlg_dlist_node_t *prev;
    mlg_dlist_node_t *next;
};

struct mlg_dlist_s
{
    mlg_dlist_node_t head;
};

mlg_error_t mlg_dlist_init(mlg_dlist_t *dlist);
mlg_error_t mlg_dlist_push_back(mlg_dlist_t *dlist, mlg_dlist_node_t *node);
mlg_error_t mlg_dlist_push_front(mlg_dlist_t *dlist, mlg_dlist_node_t *node);
mlg_error_t mlg_dlist_remove(mlg_dlist_node_t *node);

#endif // MLG_DLIST_H
