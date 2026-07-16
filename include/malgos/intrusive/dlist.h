#ifndef MLG_DLIST_H
#define MLG_DLIST_H

#include "malgos/common/types.h"

// TODO: for each macros here

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
