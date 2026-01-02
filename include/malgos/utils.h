#ifndef UTILS_H
#define UTILS_H

#define container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#endif // UTILS_H
