#include <stddef.h>
#include "malgos/common/types.h"
#include "malgos/intrusive/dlist.h"
#include "unity.h"
#include "unity_internals.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_dlist_init(void)
{
    mlg_dlist_t list;
    mlg_error_t err = mlg_dlist_init(&list);
    TEST_ASSERT_EQUAL(MLG_OK, err);
    TEST_ASSERT_EQUAL_PTR(&list.head, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, list.head.prev);
}

void test_dlist_push_back_single(void)
{
    mlg_dlist_t list;
    mlg_dlist_init(&list);

    mlg_dlist_node_t node1;
    mlg_error_t err = mlg_dlist_push_back(&list, &node1);
    TEST_ASSERT_EQUAL(MLG_OK, err);
    TEST_ASSERT_EQUAL_PTR(&node1, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node1, list.head.prev);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.prev);
}

void test_dlist_push_back_multiple(void)
{
    mlg_dlist_t list;
    mlg_dlist_init(&list);

    mlg_dlist_node_t node1, node2, node3;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_dlist_push_back(&list, &node1));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_dlist_push_back(&list, &node2));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_dlist_push_back(&list, &node3));

    TEST_ASSERT_EQUAL_PTR(&node1, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node2, node1.next);
    TEST_ASSERT_EQUAL_PTR(&node3, node2.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node3.next);

    /* reverse chain */
    TEST_ASSERT_EQUAL_PTR(&node3, list.head.prev);
    TEST_ASSERT_EQUAL_PTR(&node2, node3.prev);
    TEST_ASSERT_EQUAL_PTR(&node1, node2.prev);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.prev);
}

void test_dlist_push_front_single(void)
{
    mlg_dlist_t list;
    mlg_dlist_init(&list);

    mlg_dlist_node_t node1;
    mlg_error_t err = mlg_dlist_push_front(&list, &node1);
    TEST_ASSERT_EQUAL(MLG_OK, err);
    TEST_ASSERT_EQUAL_PTR(&node1, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node1, list.head.prev);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.prev);
}

void test_dlist_push_front_multiple(void)
{
    mlg_dlist_t list;
    mlg_dlist_init(&list);

    mlg_dlist_node_t node1, node2, node3;

    TEST_ASSERT_EQUAL(MLG_OK, mlg_dlist_push_front(&list, &node1));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_dlist_push_front(&list, &node2));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_dlist_push_front(&list, &node3));

    TEST_ASSERT_EQUAL_PTR(&node3, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node2, node3.next);
    TEST_ASSERT_EQUAL_PTR(&node1, node2.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.next);

    /* reverse chain */
    TEST_ASSERT_EQUAL_PTR(&node1, list.head.prev);
    TEST_ASSERT_EQUAL_PTR(&node2, node1.prev);
    TEST_ASSERT_EQUAL_PTR(&node3, node2.prev);
    TEST_ASSERT_EQUAL_PTR(&list.head, node3.prev);
}

void test_dlist_remove_multiple(void)
{
    mlg_dlist_t list;
    mlg_dlist_init(&list);

    mlg_dlist_node_t node1, node2, node3;
    mlg_dlist_push_back(&list, &node1);
    mlg_dlist_push_back(&list, &node2);
    mlg_dlist_push_back(&list, &node3);

    /* remove middle node */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_dlist_remove(&node2));
    TEST_ASSERT_EQUAL_PTR(&node3, node1.next);
    TEST_ASSERT_EQUAL_PTR(&node1, node3.prev);
    TEST_ASSERT_NULL(node2.next);
    TEST_ASSERT_NULL(node2.prev);

    /* remove head node */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_dlist_remove(&node1));
    TEST_ASSERT_EQUAL_PTR(&node3, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node3.prev);
    TEST_ASSERT_NULL(node1.next);
    TEST_ASSERT_NULL(node1.prev);

    /* remove last node */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_dlist_remove(&node3));
    TEST_ASSERT_EQUAL_PTR(&list.head, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, list.head.prev);
    TEST_ASSERT_NULL(node3.next);
    TEST_ASSERT_NULL(node3.prev);
}

void test_dlist_iteration_macros(void)
{
    mlg_dlist_t list;
    mlg_dlist_init(&list);

    mlg_dlist_node_t nodes[3];
    for (int i = 0; i < 3; i++)
    {
        mlg_dlist_push_back(&list, &nodes[i]);
    }

    mlg_dlist_node_t *pos;
    int count = 0;
    mlg_dlist_for_each(pos, &list)
    {
        TEST_ASSERT_EQUAL_PTR(&nodes[count], pos);
        count++;
    }
    TEST_ASSERT_EQUAL(3, count);

    mlg_dlist_node_t *n;
    count = 0;
    mlg_dlist_for_each_safe(pos, n, &list)
    {
        TEST_ASSERT_EQUAL_PTR(&nodes[count], pos);
        mlg_dlist_remove(pos);
        count++;
    }
    TEST_ASSERT_EQUAL(3, count);
    TEST_ASSERT_EQUAL_PTR(&list.head, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, list.head.prev);
}

int main(void)
{
    UnityBegin("test_intrusive_dlist.c");
    RUN_TEST(test_dlist_init);
    /* insertion tests */
    RUN_TEST(test_dlist_push_back_single);
    RUN_TEST(test_dlist_push_back_multiple);
    RUN_TEST(test_dlist_push_front_single);
    RUN_TEST(test_dlist_push_front_multiple);
    /* deletion tests */
    RUN_TEST(test_dlist_remove_multiple);
    /* iteration tests */
    RUN_TEST(test_dlist_iteration_macros);

    return UnityEnd();
}
