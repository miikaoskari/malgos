#include <stdint.h>
#include <string.h>

#include "malgos/common/hash.h"
#include "malgos/common/types.h"
#include "malgos/common/utils.h"
#include "malgos/intrusive/hashtable.h"
#include "unity.h"
#include "unity_internals.h"

void setUp(void)
{
}
void tearDown(void)
{
}

typedef struct userdata_s
{
    mlg_hash_node_t node;
    int i;
    char *str;
} userdata_t;

void test_hashtable_insert(void)
{
    userdata_t data;
    mlg_hash_head_t buckets[128];

    data.i = 42;
    data.str = "answer";

    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 128);
    TEST_ASSERT_EQUAL(MLG_OK, init_error);

    mlg_error_t error = mlg_hashtable_insert(&table, &data.node, 1);
    TEST_ASSERT_EQUAL(MLG_OK, error);

    mlg_hash_node_t *nodep;
    mlg_hash_for_each_possible(nodep, &table, 1)
    {
        TEST_ASSERT_NOT_NULL(nodep);
        userdata_t *recvdata = mlg_container_of(nodep, userdata_t, node);
        TEST_ASSERT_EQUAL_INT(data.i, recvdata->i);
        TEST_ASSERT_EQUAL_STRING(data.str, recvdata->str);
    }
}

void test_hashtable_for_each_possible(void)
{
    userdata_t first;
    userdata_t second;
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 8);
    TEST_ASSERT_EQUAL(MLG_OK, init_error);

    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &first.node, 1));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &second.node, 1));

    unsigned int count = 0;
    mlg_hash_node_t *node;
    mlg_hash_for_each_possible(node, &table, 1)
    {
        count++;
    }

    TEST_ASSERT_EQUAL_UINT(2, count);
}

void test_hashtable_for_each(void)
{
    userdata_t data[4];
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 8);
    TEST_ASSERT_EQUAL_INT(MLG_OK, init_error);

    mlg_hashtable_insert(&table, &data[0].node, 0);
    mlg_hashtable_insert(&table, &data[1].node, 1);
    mlg_hashtable_insert(&table, &data[2].node, 2);
    mlg_hashtable_insert(&table, &data[3].node, 0);

    unsigned int count = 0;
    mlg_hash_node_t *pos;
    size_t bkt;
    mlg_hash_for_each(pos, &table, bkt)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_INT(4, count);
}

void test_hashtable_for_each_safe(void)
{
    userdata_t data[4];
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 8);
    TEST_ASSERT_EQUAL_INT(MLG_OK, init_error);

    mlg_hashtable_insert(&table, &data[0].node, 0);
    mlg_hashtable_insert(&table, &data[1].node, 1);
    mlg_hashtable_insert(&table, &data[2].node, 2);
    mlg_hashtable_insert(&table, &data[3].node, 0);

    unsigned int count = 0;
    mlg_hash_node_t *pos;
    size_t bkt;
    mlg_hash_node_t *n;
    mlg_hash_for_each_safe(pos, &table, bkt, n)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_INT(4, count);
}

void test_hashtable_deletion(void)
{
    userdata_t data;
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;

    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 8);
    TEST_ASSERT_EQUAL(MLG_OK, init_error);

    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data.node, 1));

    mlg_hash_node_t *node;
    mlg_hash_for_each_possible(node, &table, 1)
    {
        TEST_ASSERT_EQUAL(&data.node, node);
    }

    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_remove(&data.node));

    int count;
    mlg_hash_for_each_possible(node, &table, 1)
    {
        count++;
    }

    TEST_ASSERT_EQUAL_INT(0, count);
}

int main(void)
{
    UnityBegin("test_intrusive_hashtable.c");
    RUN_TEST(test_hashtable_insert);
    RUN_TEST(test_hashtable_for_each_possible);
    return UnityEnd();
}
