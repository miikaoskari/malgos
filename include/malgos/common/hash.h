#ifndef MLG_HASH_H
#define MLG_HASH_H

#include <stddef.h>
#include <stdint.h>

uint32_t mlg_fnv1a_hash(const char* key, size_t length);

#endif // MLG_HASH_H
