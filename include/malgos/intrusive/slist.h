#ifndef MLG_SLIST_H
#define MLG_SLIST_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include "malgos/common/types.h"

#define mlg_slist_for_each(pos, slist) for ((pos) = (slist)->head.next; (pos) != &(slist)->head; (pos) = (pos)->next)

#define mlg_slist_for_each_safe(pos, n, slist)                                                                          \
    for ((pos) = (slist)->head.next, (n) = (pos)->next; (pos) != &(slist)->head; (pos) = (n), (n) = (pos)->next)

typedef struct mlg_slist_node_s mlg_slist_node_t;
typedef struct mlg_slist_s mlg_slist_t;

struct mlg_slist_node_s
{
    mlg_slist_node_t *next;
};

struct mlg_slist_s
{
    mlg_slist_node_t head;
    mlg_slist_node_t *tail;
};

mlg_error_t mlg_slist_init(mlg_slist_t *slist);
mlg_error_t mlg_slist_push_back(mlg_slist_t *slist, mlg_slist_node_t *node);
mlg_error_t mlg_slist_push_front(mlg_slist_t *slist, mlg_slist_node_t *node);
mlg_error_t mlg_slist_remove(mlg_slist_t *slist, mlg_slist_node_t *node);
mlg_error_t mlg_slist_insert_after(mlg_slist_t *slist, mlg_slist_node_t *node, mlg_slist_node_t *after);
mlg_error_t mlg_slist_insert_before(mlg_slist_t *slist, mlg_slist_node_t *node, mlg_slist_node_t *before);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // MLG_SLIST_H
