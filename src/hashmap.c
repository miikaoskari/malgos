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

static inline uint64_t mhash_calculate_hash(const char *value, const value_len)
{
    EVP_MD_CTX *ctx = EVP_MD_CTX_new();
    if (!ctx) return 1;
    
    uint64_t out;
    size_t outlen;

    EVP_MAC *sip = EVP_MAC_fetch(NULL, "SIPHASH", NULL);

    EVP_MAC_init(ctx, NULL, 0, NULL);
    EVP_MAC_update(ctx, value, value_len);

    size_t outl = 0;
    EVP_MAC_final(ctx, out, &outl, outlen);

    return out;
}

void mhash_create_table()
{

}

void mhash_get(struct mhash_table_s *hash_table, char *key, size_t key_len)
{
    uint64_t hash = mhash_calculate_hash(key, key_len);
    size_t idx = hash % hash_table->bucket_count;
}

void mhash_delete(struct mhash_table_s *hash_table, char *key)
{

}

int mhash_put(struct mhash_table_s *hash_table, const void *key, const size_t key_len, const void *value, const size_t value_len)
{
    uint64_t hash = mhash_calculate_hash(key, key_len);
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
