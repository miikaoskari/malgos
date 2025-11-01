#ifndef MHASHMAP_H
#define MHASHMAP_H

#include <stddef.h>
#include <openssl/evp.h>

typedef struct mhash_entry_s mhash_entry_t;

struct mhash_entry_s {
    char *key;
    size_t key_len;
    char *value;
    size_t value_len;
    mhash_entry_t *next;
};

typedef struct mhash_table_s {
    size_t bucket_count;
    mhash_entry_t **buckets;
    unsigned char key[16];
    EVP_MAC_CTX *ctx;
} mhash_table_t;

mhash_table_t mhash_create_table(char *key, size_t table_size);
void mhash_destroy_table(mhash_table_t ht);
mhash_entry_t *mhash_get(mhash_table_t *hash_table, char *key, size_t key_len);
int mhash_delete(mhash_table_t *hash_table, char *key, size_t key_len);
int mhash_put(mhash_table_t *hash_table, const void *key, const size_t key_len, const void *value, const size_t value_len);

#endif // MHASHMAP_H
