#ifndef MLG_UTILS_H
#define MLG_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

#define mlg_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLG_UTILS_H */
