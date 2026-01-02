#include "malgos/hashmap.h"

unsigned char mhash_calculate_hash(struct mhash_table_s *ht, const char *value, const size_t value_len)
{
    unsigned char out = 0;
    size_t outlen = 0;

    EVP_MAC_init(ht->ctx, ht->key, sizeof(ht->key), NULL);
    EVP_MAC_update(ht->ctx, value, value_len);
    EVP_MAC_final(ht->ctx, &out, &outlen, sizeof(out));

    return out;
}

