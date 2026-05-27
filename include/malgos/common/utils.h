#ifndef MLG_UTILS_H
#define MLG_UTILS_H

#define mlg_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#endif // UTILS_H
