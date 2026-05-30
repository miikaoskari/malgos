#ifndef MLG_UTILS_H
#define MLG_UTILS_H

#include <stddef.h>

#define mlg_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#endif // MLG_UTILS_H
