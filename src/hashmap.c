#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

static char _key[16];

typedef struct mhash_entry_s 
{
    char *key;
    size_t key_len;
    char *value;
    size_t value_len;
    mhash_entry_t *next;
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

void mhash_put(struct mhash_table_s *hash_table, void *key, size_t key_len, void *value, size_t value_len)
{
    uint64_t hash = mhash_calculate_hash(value, value_len);
    size_t idx = hash % hash_table->bucket_count;

    mhash_entry_t *e = hash_table->buckets[idx];
    while (e)
    {
        if (e->key_len == key_len && memcmp(e->key, key, key_len) == 0)
        {
            /* collision */
        }
        e = e->next;
    }

    /* new entry */
    mhash_entry_t *ne = malloc(sizeof(*ne));
    if (!ne) return 0;

    /* allocate key */
    ne->key = malloc(key_len);
    if (!ne->key) { free(ne); return 0; }
    memcpy(ne->key, key, key_len);
    ne->key_len = key_len;

    /* allocate value */
    ne->value = malloc(value_len);
    if (!ne->value) { free(ne->key); free(ne); return 0; }
    memcpy(ne->value, value, value_len);
    ne->value_len = value_len;

    /* set new entry to the bucket */
    hash_table->buckets[idx] = ne;

    return;
}
