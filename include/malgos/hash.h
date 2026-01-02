#ifndef HASH_H
#define HASH_H

#include "malgos/hashmap.h"

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

unsigned char mhash_calculate_hash(struct mhash_table_s *ht, const char *value, const size_t value_len);

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // HASH_H
