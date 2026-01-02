#include "malgos/hashmap.h"
#include <gtest/gtest.h>
#include <cstring>

class HashMapTest : public ::testing::Test {
protected:
    struct mhash_table_s *ht = nullptr;
    
    void SetUp() override {
        char test_key[16] = "test_secret_key";
        ht = mhash_create_table(test_key, 16);
    }
    
    void TearDown() override {
        mhash_destroy_table(ht);
    }
};

TEST_F(HashMapTest, CreateTable) {
    EXPECT_NE(ht->buckets, nullptr);
    EXPECT_EQ(ht->bucket_count, 16);
}

TEST_F(HashMapTest, PutAndGet) {
    const char* key = "test_key";
    const char* value = "test_value";
    
    int result = mhash_put(ht, key, strlen(key), value, strlen(value));
    EXPECT_EQ(result, 0);
    
    struct mhash_entry_s* entry = mhash_get(ht, const_cast<char*>(key), strlen(key));
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->value_len, strlen(value));
    EXPECT_EQ(memcmp(entry->value, value, strlen(value)), 0);
}

TEST_F(HashMapTest, PutMultipleEntries) {
    const char* keys[] = {"key1", "key2", "key3"};
    const char* values[] = {"value1", "value2", "value3"};
    
    for (int i = 0; i < 3; i++) {
        int result = mhash_put(ht, keys[i], strlen(keys[i]), values[i], strlen(values[i]));
        EXPECT_EQ(result, 0);
    }
    
    for (int i = 0; i < 3; i++) {
        struct mhash_entry_s* entry = mhash_get(ht, const_cast<char*>(keys[i]), strlen(keys[i]));
        ASSERT_NE(entry, nullptr);
        EXPECT_EQ(entry->value_len, strlen(values[i]));
        EXPECT_EQ(memcmp(entry->value, values[i], strlen(values[i])), 0);
    }
}

TEST_F(HashMapTest, GetNonExistentKey) {
    const char* key = "nonexistent";
    struct mhash_entry_s* entry = mhash_get(ht, const_cast<char*>(key), strlen(key));
    EXPECT_EQ(entry, nullptr);
}

TEST_F(HashMapTest, DeleteEntry) {
    const char* key = "delete_me";
    const char* value = "test_value";
    
    mhash_put(ht, key, strlen(key), value, strlen(value));
    
    int result = mhash_delete(ht, const_cast<char*>(key), strlen(key));
    EXPECT_EQ(result, 0);
    
    struct mhash_entry_s* entry = mhash_get(ht, const_cast<char*>(key), strlen(key));
    EXPECT_EQ(entry, nullptr);
}

TEST_F(HashMapTest, UpdateExistingKey) {
    const char* key = "update_key";
    const char* value1 = "original";
    const char* value2 = "updated_value";
    
    mhash_put(ht, key, strlen(key), value1, strlen(value1));
    mhash_put(ht, key, strlen(key), value2, strlen(value2));
    
    struct mhash_entry_s* entry = mhash_get(ht, const_cast<char*>(key), strlen(key));
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->value_len, strlen(value2));
    EXPECT_EQ(memcmp(entry->value, value2, strlen(value2)), 0);
}

TEST_F(HashMapTest, BinaryData) {
    const unsigned char key[] = {0x00, 0x01, 0x02, 0x03};
    const unsigned char value[] = {0xFF, 0xFE, 0xFD, 0xFC, 0xFB};
    
    int result = mhash_put(ht, key, sizeof(key), value, sizeof(value));
    EXPECT_EQ(result, 0);
    
    struct mhash_entry_s* entry = mhash_get(ht, const_cast<char*>(reinterpret_cast<const char*>(key)), sizeof(key));
    ASSERT_NE(entry, nullptr);
    EXPECT_EQ(entry->value_len, sizeof(value));
    EXPECT_EQ(memcmp(entry->value, value, sizeof(value)), 0);
}
