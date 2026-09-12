#ifndef MLG_UTILS_H
#define MLG_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#include <stddef.h>

/* clang-format off */

/**
 * @brief paste two tokens together after macro-expanding both.
 *
 * @note the two-level indirection is what lets @p b be a macro such as __LINE__.
 */
#define __mlg_paste(a, b) a##b
#define mlg_paste(a, b) __mlg_paste(a, b)

/**
 * @brief build an identifier that is unique to the line it is expanded on.
 */
#define mlg_unique_name(prefix) mlg_paste(prefix, __LINE__)

#define mlg_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - offsetof(type, member)))

#define mlg_container_of_safe(ptr, type, member) \
    __mlg_container_of_safe(ptr, type, member, mlg_unique_name(__mlg_p_))

#define __mlg_container_of_safe(ptr, type, member, p) \
    ({ typeof(ptr) p = (ptr); \
       p ? mlg_container_of(p, type, member) : NULL; \
    })

/* clang-format on */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MLG_UTILS_H */
