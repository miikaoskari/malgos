#include <stdlib.h>
#include <string.h>

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
    userdata_t data = {0};
    mlg_hash_head_t buckets[8];

    data.i = 42;
    data.str = "answer";

    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 8);
    TEST_ASSERT_EQUAL(MLG_OK, init_error);

    mlg_error_t error = mlg_hashtable_insert(&table, &data.node, 1);
    TEST_ASSERT_EQUAL(MLG_OK, error);

    userdata_t *entry;
    mlg_hash_for_each_possible(&table, entry, node, 1)
    {
        TEST_ASSERT_NOT_NULL(entry);
        TEST_ASSERT_EQUAL_INT(data.i, entry->i);
        TEST_ASSERT_EQUAL_STRING(data.str, entry->str);
    }
}

void test_hashtable_insert_invalid_table(void)
{
    userdata_t data = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_error_t error = mlg_hashtable_insert(NULL, &data.node, 1);
    TEST_ASSERT_EQUAL(MLG_ERROR, error);
}

void test_hashtable_insert_invalid_node(void)
{
    userdata_t data = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_hashtable_init(&table, buckets, 8);
    mlg_error_t error = mlg_hashtable_insert(&table, NULL, 1);
    TEST_ASSERT_EQUAL(MLG_ERROR, error);
}

void test_hashtable_node_init(void)
{
    userdata_t data;
    memset(&data, 0xAA, sizeof(data));

    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_node_init(NULL));

    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_node_init(&data.node));
    TEST_ASSERT_NULL(data.node.next);
    TEST_ASSERT_NULL(data.node.pprev);

    /* An initialized-but-never-inserted node must be rejected by remove rather than
     * dereferencing whatever pprev happened to hold. */
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_remove(&data.node));

    /* mlg_hash_node_init must produce the same state as the function */
    userdata_t initialized = { .node = mlg_hash_node_init, .i = 7, .str = NULL };
    TEST_ASSERT_NULL(initialized.node.next);
    TEST_ASSERT_NULL(initialized.node.pprev);
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_remove(&initialized.node));
}

void test_hashtable_insert_already_linked(void)
{
    userdata_t data = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_hash_table_t other_table;
    mlg_hash_head_t other_buckets[8];
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&other_table, other_buckets, 8));

    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data.node, 1));

    /* Re-inserting into the same bucket would make node->next point at the node itself,
     * turning every later traversal of that bucket into an infinite loop. */
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_insert(&table, &data.node, 1));

    /* Re-inserting into a different bucket of the same table would leave bucket 1 still
     * pointing at the node while the node only knows about bucket 2. */
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_insert(&table, &data.node, 2));

    /* Same hazard across tables: the node must be removed before it can move. */
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_insert(&other_table, &data.node, 1));

    /* The rejected inserts must not have disturbed the chain the node is really on */
    size_t count = 0;
    userdata_t *entry;
    size_t bkt;
    mlg_hash_for_each(&table, bkt, entry, node)
    {
        TEST_ASSERT_EQUAL_PTR(&data, entry);
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(1, count);

    count = 0;
    mlg_hash_for_each(&other_table, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);

    /* After a remove the node is unlinked again and may be re-inserted anywhere */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_remove(&data.node));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&other_table, &data.node, 1));
}

void test_hashtable_for_each_possible(void)
{
    userdata_t first = {0};
    userdata_t second = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 8);
    TEST_ASSERT_EQUAL(MLG_OK, init_error);

    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &first.node, 1));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &second.node, 1));

    unsigned int count = 0;
    userdata_t *entry;
    mlg_hash_for_each_possible(&table, entry, node, 1)
    {
        count++;
    }

    TEST_ASSERT_EQUAL_UINT(2, count);
}

void test_hashtable_for_each(void)
{
    userdata_t data[4] = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 8);
    TEST_ASSERT_EQUAL_INT(MLG_OK, init_error);

    mlg_hashtable_insert(&table, &data[0].node, 0);
    mlg_hashtable_insert(&table, &data[1].node, 1);
    mlg_hashtable_insert(&table, &data[2].node, 2);
    mlg_hashtable_insert(&table, &data[3].node, 0);

    unsigned int count = 0;
    userdata_t *entry;
    size_t bkt;
    mlg_hash_for_each(&table, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_INT(4, count);
}

void test_hashtable_for_each_safe(void)
{
    userdata_t data[4] = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 8);
    TEST_ASSERT_EQUAL_INT(MLG_OK, init_error);

    mlg_hashtable_insert(&table, &data[0].node, 0);
    mlg_hashtable_insert(&table, &data[1].node, 1);
    mlg_hashtable_insert(&table, &data[2].node, 2);
    mlg_hashtable_insert(&table, &data[3].node, 0);

    unsigned int count = 0;
    userdata_t *entry;
    size_t bkt;
    mlg_hash_node_t *n;
    mlg_hash_for_each_safe(&table, bkt, entry, n, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_INT(4, count);
}

void test_hashtable_deletion(void)
{
    userdata_t data = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 8);
    TEST_ASSERT_EQUAL(MLG_OK, init_error);

    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data.node, 1));

    userdata_t *entry;
    mlg_hash_for_each_possible(&table, entry, node, 1)
    {
        TEST_ASSERT_EQUAL(&data, entry);
    }

    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_remove(&data.node));

    int count = 0;
    mlg_hash_for_each_possible(&table, entry, node, 1)
    {
        count++;
    }

    TEST_ASSERT_EQUAL_INT(0, count);
}

void test_hashtable_for_each_safe_and_delete(void)
{
    userdata_t data[4] = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 8);
    TEST_ASSERT_EQUAL_INT(MLG_OK, init_error);

    mlg_hashtable_insert(&table, &data[0].node, 0);
    mlg_hashtable_insert(&table, &data[1].node, 1);
    mlg_hashtable_insert(&table, &data[2].node, 2);
    mlg_hashtable_insert(&table, &data[3].node, 0);

    unsigned int count = 0;
    userdata_t *entry;
    size_t bkt;
    mlg_hash_node_t *n;
    mlg_hash_for_each_safe(&table, bkt, entry, n, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_INT(4, count);

    count = 0;
    mlg_hash_for_each_safe(&table, bkt, entry, n, node)
    {
        mlg_hashtable_remove(&entry->node);
        count++;
    }
    TEST_ASSERT_EQUAL_INT(4, count);

    count = 0;
    mlg_hash_for_each_safe(&table, bkt, entry, n, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_INT(0, count);
}

void test_hashtable_deletion_invalid_node(void)
{
    userdata_t data[4] = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    mlg_error_t init_error = mlg_hashtable_init(&table, buckets, 8);
    TEST_ASSERT_EQUAL_INT(MLG_OK, init_error);

    mlg_hashtable_insert(&table, &data[0].node, 0);
    mlg_hashtable_insert(&table, &data[1].node, 1);
    mlg_hashtable_insert(&table, &data[2].node, 2);
    mlg_hashtable_insert(&table, &data[3].node, 0);

    /* Removing NULL */
    mlg_error_t error = mlg_hashtable_remove(NULL);
    TEST_ASSERT_EQUAL(MLG_ERROR, error);

    /* Removing uninserted node (pprev == NULL) */
    userdata_t uninserted;
    memset(&uninserted, 0, sizeof(uninserted));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_remove(&uninserted.node));

    /* Double removal: second removal must fail with MLG_ERROR */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_remove(&data[0].node));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_remove(&data[0].node));
}

size_t rehash_key_callback(mlg_hash_node_t *nodep)
{
    userdata_t *data = mlg_container_of(nodep, userdata_t, node);
    return data->i;
}

void test_hashtable_rehash_to_other_table(void)
{
    userdata_t data[4] = {0};
    mlg_hash_head_t buckets1[8];
    mlg_hash_table_t table1;
    mlg_error_t init_error = mlg_hashtable_init(&table1, buckets1, 8);
    TEST_ASSERT_EQUAL_INT(MLG_OK, init_error);

    data[0].i = 0;
    data[1].i = 1;
    data[2].i = 2;
    data[3].i = 0;

    mlg_hashtable_insert(&table1, &data[0].node, data[0].i);
    mlg_hashtable_insert(&table1, &data[1].node, data[1].i);
    mlg_hashtable_insert(&table1, &data[2].node, data[2].i);
    mlg_hashtable_insert(&table1, &data[3].node, data[3].i);

    mlg_hash_head_t buckets2[8];
    mlg_hash_table_t table2;
    init_error = mlg_hashtable_init(&table2, buckets2, 8);
    TEST_ASSERT_EQUAL_INT(MLG_OK, init_error);

    init_error = mlg_hashtable_rehash(&table1, &table2, rehash_key_callback);
    TEST_ASSERT_EQUAL_INT(MLG_OK, init_error);

    userdata_t *entry;
    size_t bkt;
    int count = 0;
    mlg_hash_for_each(&table2, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_INT(4, count);

    count = 0;
    mlg_hash_for_each(&table1, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_INT(0, count);
}

void test_hashtable_init_validation(void)
{
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;

    /* NULL table pointer */
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_init(NULL, buckets, 8));

    /* NULL buckets pointer */
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_init(&table, NULL, 8));

    /* Zero size */
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_init(&table, buckets, 0));

    /* Non-power of 2 sizes */
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_init(&table, buckets, 3));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_init(&table, buckets, 5));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_init(&table, buckets, 6));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_init(&table, buckets, 7));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_init(&table, buckets, 9));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_init(&table, buckets, 10));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_init(&table, buckets, 12));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_init(&table, buckets, 15));

    /* Valid power of 2 sizes */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 1));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 2));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 4));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));
}

void test_hashtable_insert_invalid_size(void)
{
    userdata_t data = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table = { .buckets = buckets, .size = 0 };

    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_insert(&table, &data.node, 1));
}

void test_hashtable_collision_chain_and_removal(void)
{
    userdata_t data[4] = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));

    /* All keys hash to bucket (key & 7) == 1: keys 1, 9, 17, 25 */
    for (int i = 0; i < 4; i++)
    {
        data[i].i = 1 + i * 8;
        TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data[i].node, data[i].i));
    }

    /* Verify all 4 are in bucket 1 */
    size_t count = 0;
    userdata_t *entry;
    mlg_hash_for_each_possible(&table, entry, node, 1)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(4, count);

    /* Remove a middle node in the collision chain: data[2] (key 17) */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_remove(&data[2].node));
    TEST_ASSERT_NULL(data[2].node.next);
    TEST_ASSERT_NULL(data[2].node.pprev);

    count = 0;
    mlg_hash_for_each_possible(&table, entry, node, 1)
    {
        TEST_ASSERT_NOT_EQUAL(data[2].i, entry->i);
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(3, count);

    /* Remove the current head node: data[3] (key 25) */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_remove(&data[3].node));
    count = 0;
    mlg_hash_for_each_possible(&table, entry, node, 1)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(2, count);

    /* Remove the tail node: data[0] (key 1) */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_remove(&data[0].node));
    count = 0;
    mlg_hash_for_each_possible(&table, entry, node, 1)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(1, count);

    /* Remove the last remaining node: data[1] (key 9) */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_remove(&data[1].node));
    count = 0;
    mlg_hash_for_each_possible(&table, entry, node, 1)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);
}

void test_hashtable_for_each_empty_table(void)
{
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));

    size_t count = 0;
    userdata_t *entry;
    size_t bkt;
    mlg_hash_for_each(&table, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);

    count = 0;
    mlg_hash_node_t *n;
    mlg_hash_for_each_safe(&table, bkt, entry, n, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);

    count = 0;
    mlg_hash_for_each_possible(&table, entry, node, 3)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);
}

void test_hashtable_for_each_possible_expression_key(void)
{
    userdata_t data = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));

    data.i = 3;
    data.str = "expr";
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data.node, 3));

    /* Test expressions with bitwise operators that have lower precedence than subtraction */
    size_t count = 0;
    userdata_t *entry;
    mlg_hash_for_each_possible(&table, entry, node, 1 | 2)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(1, count);

    count = 0;
    mlg_hash_for_each_possible(&table, entry, node, 7 & 3)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(1, count);
}

static int s_eval_count = 0;
static mlg_hash_table_t *count_table_eval(mlg_hash_table_t *t)
{
    s_eval_count++;
    return t;
}

void test_hashtable_for_each_break(void)
{
    userdata_t data[2] = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));

    data[0].i = 0; /* bucket 0 */
    data[1].i = 1; /* bucket 1 */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data[0].node, data[0].i));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data[1].node, data[1].i));

    /* Break should terminate traversal across all buckets, visiting exactly 1 item */
    size_t count = 0;
    userdata_t *entry;
    size_t bkt;
    mlg_hash_for_each(&table, bkt, entry, node)
    {
        count++;
        break;
    }
    TEST_ASSERT_EQUAL_UINT(1, count);
}

void test_hashtable_for_each_safe_break(void)
{
    userdata_t data[2] = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));

    data[0].i = 0; /* bucket 0 */
    data[1].i = 1; /* bucket 1 */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data[0].node, data[0].i));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data[1].node, data[1].i));

    /* Break in safe iteration should terminate traversal across all buckets */
    size_t count = 0;
    userdata_t *entry;
    size_t bkt;
    mlg_hash_node_t *n;
    mlg_hash_for_each_safe(&table, bkt, entry, n, node)
    {
        count++;
        break;
    }
    TEST_ASSERT_EQUAL_UINT(1, count);
}

void test_hashtable_for_each_safe_heap_free(void)
{
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));

    /* Allocate nodes dynamically on the heap */
    for (int i = 0; i < 8; i++)
    {
        userdata_t *item = (userdata_t *)malloc(sizeof(userdata_t));
        TEST_ASSERT_NOT_NULL(item);
        item->i = i;
        item->str = NULL;
        TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_node_init(&item->node));
        TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &item->node, item->i));
    }

    /* Remove and free each item inside safe iteration; ASan verifies no use-after-free */
    size_t count = 0;
    userdata_t *entry;
    size_t bkt;
    mlg_hash_node_t *n;
    mlg_hash_for_each_safe(&table, bkt, entry, n, node)
    {
        TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_remove(&entry->node));
        free(entry);
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(8, count);

    /* Table must now be completely empty */
    count = 0;
    mlg_hash_for_each(&table, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);
}

void test_hashtable_for_each_safe_remove_partial(void)
{
    userdata_t data[8] = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));

    for (int i = 0; i < 8; i++)
    {
        data[i].i = i;
        TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data[i].node, data[i].i));
    }

    /* Remove only even-keyed items during iteration */
    userdata_t *entry;
    size_t bkt;
    mlg_hash_node_t *n;
    mlg_hash_for_each_safe(&table, bkt, entry, n, node)
    {
        if (entry->i % 2 == 0)
        {
            TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_remove(&entry->node));
        }
    }

    /* Verify exactly 4 odd-keyed items remain and are intact */
    size_t count = 0;
    mlg_hash_for_each(&table, bkt, entry, node)
    {
        TEST_ASSERT_EQUAL_INT(1, entry->i % 2);
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(4, count);
}

void test_hashtable_nested_iteration(void)
{
    userdata_t data[4] = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));

    for (int i = 0; i < 4; i++)
    {
        data[i].i = i;
        TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data[i].node, data[i].i));
    }

    /* Nesting two traversals must give each its own scratch state; the macros used to share
     * fixed __mlg_tbl / __mlg_head names, so the inner loop shadowed the outer one's. */
    size_t pairs = 0;
    userdata_t *outer;
    userdata_t *inner;
    size_t outer_bkt;
    size_t inner_bkt;
    mlg_hash_for_each(&table, outer_bkt, outer, node)
    {
        mlg_hash_for_each(&table, inner_bkt, inner, node)
        {
            pairs++;
        }
        /* the inner traversal must not have disturbed the outer cursor */
        TEST_ASSERT_NOT_NULL(outer);
    }
    TEST_ASSERT_EQUAL_UINT(16, pairs);

    /* Same check for the safe and bucket-scoped variants */
    pairs = 0;
    mlg_hash_node_t *outer_n;
    mlg_hash_node_t *inner_n;
    mlg_hash_for_each_safe(&table, outer_bkt, outer, outer_n, node)
    {
        mlg_hash_for_each_possible_safe(&table, inner, inner_n, node, outer->i)
        {
            pairs++;
        }
    }
    TEST_ASSERT_EQUAL_UINT(4, pairs);
}

void test_hashtable_for_each_possible_macro_single_evaluation(void)
{
    userdata_t data = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));

    data.i = 1;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data.node, 1));

    s_eval_count = 0;
    userdata_t *entry;
    mlg_hash_for_each_possible(count_table_eval(&table), entry, node, 1)
    {
        (void)entry;
    }
    /* Macro must evaluate its table argument exactly once */
    TEST_ASSERT_EQUAL_INT(1, s_eval_count);
}

void test_hashtable_for_each_macro_single_evaluation(void)
{
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));

    s_eval_count = 0;
    userdata_t *entry;
    size_t bkt;
    mlg_hash_for_each(count_table_eval(&table), bkt, entry, node)
    {
        (void)entry;
    }
    /* Table pointer must not be evaluated on every loop condition check */
    TEST_ASSERT_EQUAL_INT(1, s_eval_count);
}

void test_hashtable_for_each_possible_zero_size(void)
{
    /* Table with size == 0 or uninitialized */
    mlg_hash_head_t bucket;
    bucket.first = NULL;
    mlg_hash_table_t table = { .buckets = &bucket, .size = 0 };

    /* when size == 0, (size - 1) underflows to SIZE_MAX and performs an out-of-bounds read
     * at table.buckets[42], which triggers an AddressSanitizer SEGV/stack-buffer-overflow.
     * When guarded against size == 0, pos is initialized to NULL and loop body does not run. */
    size_t count = 0;
    userdata_t *entry;
    mlg_hash_for_each_possible(&table, entry, node, 42)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);
}

void test_hashtable_for_each_possible_null_table(void)
{
    userdata_t *entry = NULL;
    size_t count = 0;

    /* 1. Regression test: NULL table pointer in mlg_hash_for_each_possible */
    mlg_hash_for_each_possible(NULL, entry, node, 42)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);

    /* 2. Regression test: NULL table pointer in mlg_hash_for_each_possible_safe */
    count = 0;
    mlg_hash_node_t *n;
    mlg_hash_for_each_possible_safe(NULL, entry, n, node, 42)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);

    /* 3. Regression test: Non-null table with NULL buckets and non-zero size */
    mlg_hash_table_t null_bkt_table = { .buckets = NULL, .size = 8 };
    count = 0;
    mlg_hash_for_each_possible(&null_bkt_table, entry, node, 42)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);

    count = 0;
    mlg_hash_for_each_possible_safe(&null_bkt_table, entry, n, node, 42)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);
}

void test_hashtable_for_each_null_buckets(void)
{
    mlg_hash_table_t null_bkt_table = { .buckets = NULL, .size = 8 };
    userdata_t *entry = NULL;
    size_t bkt;
    size_t count = 0;

    /* 1. Regression test: mlg_hash_for_each with NULL buckets and size > 0 */
    mlg_hash_for_each(&null_bkt_table, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);

    /* 2. Regression test: mlg_hash_for_each_safe with NULL buckets and size > 0 */
    count = 0;
    mlg_hash_node_t *n;
    mlg_hash_for_each_safe(&null_bkt_table, bkt, entry, n, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);

    /* 3. Regression test: mlg_hash_for_each_node_safe with NULL buckets and size > 0 */
    count = 0;
    mlg_hash_node_t *pos;
    mlg_hash_for_each_node_safe(&null_bkt_table, bkt, pos, n)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);

    /* 4. Regression test: mlg_hash_for_each with NULL table */
    count = 0;
    mlg_hash_for_each(NULL, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);
}

void test_hashtable_rehash_null_arguments(void)
{
    mlg_hash_head_t buckets1[8];
    mlg_hash_head_t buckets2[8];
    mlg_hash_table_t table1, table2;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table1, buckets1, 8));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table2, buckets2, 8));

    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_rehash(NULL, &table2, rehash_key_callback));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_rehash(&table1, NULL, rehash_key_callback));
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_rehash(&table1, &table2, NULL));
}

void test_hashtable_rehash_self(void)
{
    userdata_t data[2] = {0};
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table, buckets, 8));

    data[0].i = 1;
    data[1].i = 2;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data[0].node, data[0].i));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table, &data[1].node, data[1].i));

    /* In-place rehashing must be rejected to prevent infinite loops and pointer corruption */
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_rehash(&table, &table, rehash_key_callback));
}

void test_hashtable_rehash_invalid_source_table(void)
{
    mlg_hash_head_t buckets[8];
    mlg_hash_table_t table2;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table2, buckets, 8));

    /* Uninitialized / zero-size source table */
    mlg_hash_table_t invalid_src1 = { .buckets = buckets, .size = 0 };
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_rehash(&invalid_src1, &table2, rehash_key_callback));

    mlg_hash_table_t invalid_src2 = { .buckets = NULL, .size = 0 };
    TEST_ASSERT_EQUAL(MLG_ERROR, mlg_hashtable_rehash(&invalid_src2, &table2, rehash_key_callback));
}

void test_hashtable_rehash_invalid_dest_table_preserves_nodes(void)
{
    userdata_t data[2] = {0};
    mlg_hash_head_t buckets1[8];
    mlg_hash_table_t table1;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table1, buckets1, 8));

    data[0].i = 1;
    data[1].i = 2;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table1, &data[0].node, data[0].i));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table1, &data[1].node, data[1].i));

    /* rehash into invalid destination table must return MLG_ERROR */
    mlg_hash_table_t invalid_dst = { .buckets = NULL, .size = 0 };
    mlg_error_t err = mlg_hashtable_rehash(&table1, &invalid_dst, rehash_key_callback);
    TEST_ASSERT_EQUAL(MLG_ERROR, err);

    /* nodes in old_table must NOT be removed or lost when rehash fails */
    size_t count = 0;
    userdata_t *entry;
    size_t bkt;
    mlg_hash_for_each(&table1, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(2, count);
}

void test_hashtable_rehash_grow_and_shrink(void)
{
    userdata_t data[8] = {0};
    mlg_hash_head_t buckets4[4];
    mlg_hash_head_t buckets16[16];
    mlg_hash_head_t buckets4_final[4];
    mlg_hash_table_t table4, table16, table4_final;

    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table4, buckets4, 4));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table16, buckets16, 16));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_init(&table4_final, buckets4_final, 4));

    for (int i = 0; i < 8; i++)
    {
        data[i].i = i;
        TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_insert(&table4, &data[i].node, data[i].i));
    }

    /* Grow: rehash from 4 to 16 buckets */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_rehash(&table4, &table16, rehash_key_callback));

    /* Old table must be empty */
    size_t count = 0;
    userdata_t *entry;
    size_t bkt;
    mlg_hash_for_each(&table4, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);

    /* All 8 items must be in table16 and findable */
    count = 0;
    mlg_hash_for_each(&table16, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(8, count);

    for (int i = 0; i < 8; i++)
    {
        int found = 0;
        mlg_hash_for_each_possible(&table16, entry, node, i)
        {
            if (entry->i == i)
            {
                found = 1;
                break;
            }
        }
        TEST_ASSERT_TRUE(found);
    }

    /* Shrink: rehash from 16 back to 4 buckets */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_hashtable_rehash(&table16, &table4_final, rehash_key_callback));

    /* table16 must now be empty */
    count = 0;
    mlg_hash_for_each(&table16, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(0, count);

    /* All 8 items must be in table4_final and findable */
    count = 0;
    mlg_hash_for_each(&table4_final, bkt, entry, node)
    {
        count++;
    }
    TEST_ASSERT_EQUAL_UINT(8, count);

    for (int i = 0; i < 8; i++)
    {
        int found = 0;
        mlg_hash_for_each_possible(&table4_final, entry, node, i)
        {
            if (entry->i == i)
            {
                found = 1;
                break;
            }
        }
        TEST_ASSERT_TRUE(found);
    }
}

int main(void)
{
    UnityBegin("test_intrusive_hashtable.c");
    /* initialization tests */
    RUN_TEST(test_hashtable_init_validation);
    RUN_TEST(test_hashtable_node_init);

    /* insertion tests */
    RUN_TEST(test_hashtable_insert);
    RUN_TEST(test_hashtable_insert_invalid_table);
    RUN_TEST(test_hashtable_insert_invalid_node);
    RUN_TEST(test_hashtable_insert_invalid_size);
    RUN_TEST(test_hashtable_insert_already_linked);
    RUN_TEST(test_hashtable_collision_chain_and_removal);

    /* iteration tests */
    RUN_TEST(test_hashtable_for_each_possible);
    RUN_TEST(test_hashtable_for_each_possible_expression_key);
    RUN_TEST(test_hashtable_for_each_possible_macro_single_evaluation);
    RUN_TEST(test_hashtable_for_each);
    RUN_TEST(test_hashtable_for_each_macro_single_evaluation);
    RUN_TEST(test_hashtable_for_each_break);
    RUN_TEST(test_hashtable_for_each_safe);
    RUN_TEST(test_hashtable_for_each_safe_break);
    RUN_TEST(test_hashtable_for_each_safe_heap_free);
    RUN_TEST(test_hashtable_for_each_safe_remove_partial);
    RUN_TEST(test_hashtable_for_each_empty_table);
    RUN_TEST(test_hashtable_nested_iteration);
    RUN_TEST(test_hashtable_for_each_possible_zero_size);
    RUN_TEST(test_hashtable_for_each_possible_null_table);
    RUN_TEST(test_hashtable_for_each_null_buckets);

    /* deletion tests */
    RUN_TEST(test_hashtable_deletion);
    RUN_TEST(test_hashtable_for_each_safe_and_delete);
    RUN_TEST(test_hashtable_deletion_invalid_node);

    /* rehashing tests */
    RUN_TEST(test_hashtable_rehash_to_other_table);
    RUN_TEST(test_hashtable_rehash_null_arguments);
    RUN_TEST(test_hashtable_rehash_self);
    RUN_TEST(test_hashtable_rehash_invalid_source_table);
    RUN_TEST(test_hashtable_rehash_invalid_dest_table_preserves_nodes);
    RUN_TEST(test_hashtable_rehash_grow_and_shrink);

    return UnityEnd();
}
