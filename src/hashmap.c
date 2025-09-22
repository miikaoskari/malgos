#include <stdlib.h>

#include "siphash.h"

static char _key[16];

typedef struct mhash_entry_s 
{
    char *key;
    char *value;
} mhash_entry_t;

struct mhash_table_s
{
    size_t bucket_count;
    mhash_entry_t **buckets;
};

size_t mhash_init_key(char *key)
{
    if (!key) return 0;
    memcpy(_key, key, sizeof(_key));
    return sizeof(_key);
}

uint64_t mhash_calculate_hash(char *key)
{
    /* siphash-2-4 */
    uint64_t hash_result = 0;
    siphash((const uint8_t *)key, strlen(key), (const uint8_t *)_key, (uint8_t *)&hash_result, sizeof(hash_result));
    return hash_result;
}

void mhash_create_table()
{

}

void mhash_get(struct mhash_table_s *hash_table, char *key)
{
    uint64_t hash = mhash_calculate_hash(key);
    size_t idx = hash % hash_table->bucket_count;
}

void mhash_delete(struct mhash_table_s *hash_table, char *key)
{

}

void mhash_set(struct mhash_table_s *hash_table, char *key, void *value)
{
    uint64_t hash = mhash_calculate_hash(key);





}
