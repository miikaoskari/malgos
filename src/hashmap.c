#include "malgos/hashmap.h"
#include "malgos/hash.h"
#include <openssl/evp.h>
#include <stdlib.h>
#include <string.h>

static size_t mhash_init_key(struct mhash_table_s *ht, char *key)
{
    if (!key)
        return 0;
    memcpy(ht->key, key, sizeof(ht->key));
    return sizeof(ht->key);
}

static size_t mhash_init_siphash_ctx(struct mhash_table_s *ht)
{
    EVP_MAC *sip = EVP_MAC_fetch(NULL, "SIPHASH", NULL);
    if (!sip)
        return 1;
    ht->ctx = EVP_MAC_CTX_new(sip);

    if (!ht->ctx)
    {
        EVP_MAC_CTX_free(ht->ctx);
        return 1;
    }

    return 0;
}

struct mhash_table_s *mhash_create_table(char *key, size_t table_size)
{
    struct mhash_table_s *ht = calloc(1, sizeof(*ht));
    if (!ht)
        return NULL;

    ht->bucket_count = table_size;
    ht->buckets = calloc(ht->bucket_count, sizeof(*ht->buckets));

    if (!ht->buckets)
    {
        free(ht);
        return NULL;
    }

    if (mhash_init_key(ht, key) == 0)
    {
        free(ht->buckets);
        free(ht);
        return NULL;
    }

    if (mhash_init_siphash_ctx(ht) != 0)
    {
        free(ht->buckets);
        free(ht);
        return NULL;
    }

    return ht;
}

void mhash_destroy_table(struct mhash_table_s *ht)
{
    if (ht->buckets)
    {
        for (size_t i = 0; i < ht->bucket_count; ++i)
        {
            struct mhash_entry_s *e = ht->buckets[i];
            while (e)
            {
                struct mhash_entry_s *next = e->next;
                free(e->key);
                free(e->value);
                free(e);
                e = next;
            }
        }
        free(ht->buckets);
    }
    if (ht->ctx)
    {
        EVP_MAC_CTX_free(ht->ctx);
    }
    return;
}

struct mhash_entry_s *mhash_get(struct mhash_table_s *hash_table, char *key, size_t key_len)
{
    uint64_t hash = mhash_calculate_hash(hash_table, key, key_len);
    size_t idx = hash % hash_table->bucket_count;

    struct mhash_entry_s *e = hash_table->buckets[idx];
    while (e)
    {
        if (e->key_len == key_len && memcmp(e->key, key, key_len) == 0)
        {
            /* key found! */
            return e;
        }

        e = e->next;
    }

    /* key not found */
    return NULL;
}

int mhash_delete(struct mhash_table_s *hash_table, char *key, size_t key_len)
{
    struct mhash_entry_s *e = mhash_get(hash_table, key, key_len);

    if (e)
    {
        free(e->value);
        free(e->key);
        free(e);
        return 0;
    }

    return 1;
}

int mhash_put(struct mhash_table_s *hash_table, const void *key, const size_t key_len, const void *value,
              const size_t value_len)
{
    uint64_t hash = mhash_calculate_hash(hash_table, key, key_len);
    size_t idx = hash % hash_table->bucket_count;

    struct mhash_entry_s *e = hash_table->buckets[idx];
    while (e)
    {
        if (e->key_len == key_len && memcmp(e->key, key, key_len) == 0)
        {
            /* same key! replace with new value */
            char *new_value = realloc(e->value, value_len);
            if (!new_value)
                return 1;
            e->value = new_value;
            memcpy(e->value, value, value_len);
            e->value_len = value_len;
            return 0;
        }

        /* continue along the chain */
        e = e->next;
    }

    /* new entry */
    struct mhash_entry_s *ne = malloc(sizeof(*ne));
    if (!ne)
        return 1;

    /* allocate key */
    ne->key = malloc(key_len);
    if (!ne->key)
    {
        free(ne);
        return 1;
    }
    memcpy(ne->key, key, key_len);
    ne->key_len = key_len;

    /* allocate value */
    ne->value = malloc(value_len);
    if (!ne->value)
    {
        free(ne->key);
        free(ne);
        return 1;
    }
    memcpy(ne->value, value, value_len);
    ne->value_len = value_len;

    /* set new entry to the bucket */
    ne->next = hash_table->buckets[idx];
    hash_table->buckets[idx] = ne;

    return 0;
}
