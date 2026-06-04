#include "malgos/common/types.h"
#include "malgos/common/utils.h"
#include "malgos/intrusive/hashtable.h"
#include "malgos/common/hash.h"
#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

typedef struct userdata_s {
    mlg_hash_node_t node;
    int i;
    char* str;
} userdata_t;

void test_hashtable_init(void) {
    userdata_t data;
    mlg_hash_node_t *buckets[128];

    data.i = 42;
    data.str = "answer";

    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 128);
    TEST_ASSERT_EQUAL(MLG_OK, init_error);

    data.node.hash = mlg_fnv1a_hash("testkey", sizeof("testkey"));
    mlg_error_t error = mlg_hashtable_insert(&table, &data.node);
    TEST_ASSERT_EQUAL(MLG_OK, error);

    mlg_hash_node_t *nodep = mlg_hashtable_get(&table, data.node.hash);
    TEST_ASSERT_NOT_NULL(nodep);
    userdata_t *recvdata = mlg_container_of(nodep, userdata_t, node);
    TEST_ASSERT_EQUAL_INT(data.i, recvdata->i);
    TEST_ASSERT_EQUAL_STRING(data.str, recvdata->str);

    mlg_error_t remove_error = mlg_hashtable_remove(&table, &data.node);
    TEST_ASSERT_EQUAL(MLG_OK, remove_error);
    TEST_ASSERT_NULL(mlg_hashtable_get(&table, data.node.hash));
}


int main(void) {
    UnityBegin("test_intrusive_hashtable.c");
    RUN_TEST(test_hashtable_init);
    return UnityEnd();
}
