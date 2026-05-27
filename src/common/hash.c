#include <stddef.h>
#include <stdint.h>

uint32_t mlg_fnv1a_hash(const char* key, size_t length) {
    uint32_t hash = 2166136261u;
    for (size_t i = 0; i < length; ++i) {
        hash ^= (uint8_t)key[i];
        hash *= 16777619u;
    }
    return hash;
}
