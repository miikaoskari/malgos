#ifndef MHASHMAP_H
#define MHASHMAP_H

#include <stddef.h>
#include <openssl/evp.h>

#ifdef __cplusplus
extern "C" {
#endif

struct mhash_entry_s {
    char *key;
    size_t key_len;
    char *value;
    size_t value_len;
    struct mhash_entry_s *next;
};

struct mhash_table_s {
    size_t bucket_count;
    struct mhash_entry_s **buckets;
    unsigned char key[16];
    EVP_MAC_CTX *ctx;
};

struct mhash_table_s *mhash_create_table(char *key, size_t table_size);
void mhash_destroy_table(struct mhash_table_s *ht);
struct mhash_entry_s *mhash_get(struct mhash_table_s *hash_table, char *key, size_t key_len);
int mhash_delete(struct mhash_table_s *hash_table, char *key, size_t key_len);
int mhash_put(struct mhash_table_s *hash_table, const void *key, const size_t key_len, const void *value, const size_t value_len);

#ifdef __cplusplus
}
#endif

#endif // MHASHMAP_H
