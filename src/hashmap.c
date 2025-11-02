#include <stdlib.h>
#include <string.h>
#include <openssl/evp.h>

typedef struct mhash_entry_s mhash_entry_t;

struct mhash_entry_s 
{
    char *key;
    size_t key_len;
    char *value;
    size_t value_len;
    mhash_entry_t *next;
};

typedef struct mhash_table_s
{
    size_t bucket_count;
    mhash_entry_t **buckets;
    unsigned char key[16];

    EVP_MAC_CTX *ctx;
} mhash_table_t;

static size_t mhash_init_key(mhash_table_t *ht, char *key)
{
    if (!key) return 0;
    memcpy(ht->key, key, sizeof(ht->key));
    return sizeof(ht->key);
}

static size_t mhash_init_siphash_ctx(mhash_table_t *ht)
{
    EVP_MAC *sip = EVP_MAC_fetch(NULL, "SIPHASH", NULL);
    EVP_MAC_CTX *ctx = EVP_MAC_CTX_new(sip);

    if (!EVP_MAC_init(ctx, ht->key, sizeof(ht->key), NULL))
    {
        return 1;
    }
}

static inline unsigned char mhash_calculate_hash(mhash_table_t *ht, const char *value, const size_t value_len)
{
    unsigned char out;
    size_t outlen;

    EVP_MAC_update(ht->ctx, value, value_len);
    EVP_MAC_final(ht->ctx, &out, &outlen, sizeof(out));

    return out;
}

mhash_table_t *mhash_create_table(char *key, size_t table_size)
{
    mhash_table_t *ht = calloc(1, sizeof(*ht));
    if (!ht) return NULL;

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

void mhash_destroy_table(mhash_table_t *ht)
{
    if (ht->buckets)
    {
        for (size_t i = 0; i < ht->bucket_count; ++i)
        {
            mhash_entry_t *e = ht->buckets[i];
            while (e)
            {
                mhash_entry_t *next = e->next;
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

mhash_entry_t *mhash_get(mhash_table_t *hash_table, char *key, size_t key_len)
{
    uint64_t hash = mhash_calculate_hash(hash_table, key, key_len);
    size_t idx = hash % hash_table->bucket_count;

    mhash_entry_t *e = hash_table->buckets[idx];
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

int mhash_delete(mhash_table_t *hash_table, char *key, size_t key_len)
{
    mhash_entry_t *e = mhash_get(hash_table, key, key_len);

    if (e)
    {
        free(e->value);
        free(e->key);
        free(e);
        return 0;
    }

    return 1; 
}

int mhash_put(mhash_table_t *hash_table, const void *key, const size_t key_len, const void *value, const size_t value_len)
{
    uint64_t hash = mhash_calculate_hash(hash_table, key, key_len);
    size_t idx = hash % hash_table->bucket_count;

    mhash_entry_t *e = hash_table->buckets[idx];
    while (e)
    {
        if (e->key_len == key_len && memcmp(e->key, key, key_len) == 0)
        {
            /* same key! replace with new value */
            char *new_value = realloc(e->value, value_len);
            if (!new_value) return 1;
            e->value = new_value;
            memcpy(e->value, value, value_len);
            e->value_len = value_len;
            return 0;
        }

        /* continue along the chain */
        e = e->next;
    }

    /* new entry */
    mhash_entry_t *ne = malloc(sizeof(*ne));
    if (!ne) return 1;

    /* allocate key */
    ne->key = malloc(key_len);
    if (!ne->key) { free(ne); return 1; }
    memcpy(ne->key, key, key_len);
    ne->key_len = key_len;

    /* allocate value */
    ne->value = malloc(value_len);
    if (!ne->value) { free(ne->key); free(ne); return 1; }
    memcpy(ne->value, value, value_len);
    ne->value_len = value_len;

    /* set new entry to the bucket */
    ne->next = hash_table->buckets[idx];
    hash_table->buckets[idx] = ne;

    return 0;
}
