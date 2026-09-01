#include <stddef.h>
#include "malgos/common/types.h"
#include "malgos/intrusive/slist.h"
#include "unity.h"
#include "unity_internals.h"

void setUp(void)
{
}

void tearDown(void)
{
}

void test_slist_init(void)
{
    mlg_slist_t list;
    mlg_error_t err = mlg_slist_init(&list);
    TEST_ASSERT_EQUAL(MLG_OK, err);
    TEST_ASSERT_EQUAL_PTR(&list.head, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, list.tail);
}

void test_slist_push_back_single(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t node1;
    mlg_error_t err = mlg_slist_push_back(&list, &node1);
    TEST_ASSERT_EQUAL(MLG_OK, err);
    TEST_ASSERT_EQUAL_PTR(&node1, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.next);
    TEST_ASSERT_EQUAL_PTR(&node1, list.tail);
}

void test_slist_push_back_multiple(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t node1, node2, node3;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_back(&list, &node1));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_back(&list, &node2));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_back(&list, &node3));

    TEST_ASSERT_EQUAL_PTR(&node1, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node2, node1.next);
    TEST_ASSERT_EQUAL_PTR(&node3, node2.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node3.next);

    TEST_ASSERT_EQUAL_PTR(&node3, list.tail);
}

void test_slist_push_front_single(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t node1;
    mlg_error_t err = mlg_slist_push_front(&list, &node1);
    TEST_ASSERT_EQUAL(MLG_OK, err);
    TEST_ASSERT_EQUAL_PTR(&node1, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.next);
    TEST_ASSERT_EQUAL_PTR(&node1, list.tail);
}

void test_slist_push_front_multiple(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t node1, node2, node3;

    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_front(&list, &node1));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_front(&list, &node2));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_front(&list, &node3));

    TEST_ASSERT_EQUAL_PTR(&node3, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node2, node3.next);
    TEST_ASSERT_EQUAL_PTR(&node1, node2.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.next);

    /* only the first push_front (on an empty list) sets the tail */
    TEST_ASSERT_EQUAL_PTR(&node1, list.tail);
}

void test_slist_insert_after(void)
{
    mlg_slist_t list;

    mlg_slist_init(&list);

    mlg_slist_node_t node1, node2, node3, after_node_2;

    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_front(&list, &node1));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_front(&list, &node2));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_front(&list, &node3));

    /* insert after node 2 */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_insert_after(&list, &after_node_2, &node2));

    /* check the order */
    TEST_ASSERT_EQUAL_PTR(&node3, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node2, node3.next);
    TEST_ASSERT_EQUAL_PTR(&after_node_2, node2.next);
    TEST_ASSERT_EQUAL_PTR(&node1, after_node_2.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.next);

    TEST_ASSERT_EQUAL_PTR(&node1, list.tail);
}

void test_slist_insert_before(void)
{
    mlg_slist_t list;

    mlg_slist_init(&list);

    mlg_slist_node_t node1, node2, node3, before_node_2;

    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_front(&list, &node1));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_front(&list, &node2));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_front(&list, &node3));

    /* insert before node 2 */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_insert_before(&list, &before_node_2, &node2));

    /* check the order */
    TEST_ASSERT_EQUAL_PTR(&node3, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&before_node_2, node3.next);
    TEST_ASSERT_EQUAL_PTR(&node2, before_node_2.next);
    TEST_ASSERT_EQUAL_PTR(&node1, node2.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.next);

    TEST_ASSERT_EQUAL_PTR(&node1, list.tail);
}

void test_slist_insert_after_at_tail(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t node1, node2, new_node;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_back(&list, &node1));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_back(&list, &node2));

    /* node2 is currently the tail. inserting after it must become the new tail */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_insert_after(&list, &new_node, &node2));

    TEST_ASSERT_EQUAL_PTR(&node1, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node2, node1.next);
    TEST_ASSERT_EQUAL_PTR(&new_node, node2.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, new_node.next);

    TEST_ASSERT_EQUAL_PTR(&new_node, list.tail);
}

void test_slist_insert_before_at_head(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t node1, node2, new_node;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_back(&list, &node1));
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_back(&list, &node2));

    /* node1 is currently the head. inserting before it must become the new head */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_insert_before(&list, &new_node, &node1));

    TEST_ASSERT_EQUAL_PTR(&new_node, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node1, new_node.next);
    TEST_ASSERT_EQUAL_PTR(&node2, node1.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node2.next);

    /* tail is unaffected */
    TEST_ASSERT_EQUAL_PTR(&node2, list.tail);
}

void test_slist_insert_after_sentinel_matches_push_front(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t node1, new_node;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_back(&list, &node1));

    /* the list head is itself a plain node, so inserting after it is
     * equivalent to push_front */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_insert_after(&list, &new_node, &list.head));

    TEST_ASSERT_EQUAL_PTR(&new_node, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node1, new_node.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.next);

    TEST_ASSERT_EQUAL_PTR(&node1, list.tail);
}

void test_slist_insert_before_sentinel_matches_push_back(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t node1, new_node;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_back(&list, &node1));

    /* the list head is itself a plain node, so inserting before it is
     * equivalent to push_back */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_insert_before(&list, &new_node, &list.head));

    TEST_ASSERT_EQUAL_PTR(&node1, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&new_node, node1.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, new_node.next);

    TEST_ASSERT_EQUAL_PTR(&new_node, list.tail);
}

void test_slist_insert_after_single_element_list(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t node1, new_node;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_back(&list, &node1));

    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_insert_after(&list, &new_node, &node1));

    TEST_ASSERT_EQUAL_PTR(&node1, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&new_node, node1.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, new_node.next);

    TEST_ASSERT_EQUAL_PTR(&new_node, list.tail);
}

void test_slist_insert_before_single_element_list(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t node1, new_node;
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_push_back(&list, &node1));

    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_insert_before(&list, &new_node, &node1));

    TEST_ASSERT_EQUAL_PTR(&new_node, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node1, new_node.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, node1.next);

    TEST_ASSERT_EQUAL_PTR(&node1, list.tail);
}

void test_slist_remove_multiple(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t node1, node2, node3;
    mlg_slist_push_back(&list, &node1);
    mlg_slist_push_back(&list, &node2);
    mlg_slist_push_back(&list, &node3);

    /* remove middle node */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_remove(&list, &node2));
    TEST_ASSERT_EQUAL_PTR(&node3, node1.next);
    TEST_ASSERT_NULL(node2.next);
    TEST_ASSERT_EQUAL_PTR(&node3, list.tail);

    /* remove head node */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_remove(&list, &node1));
    TEST_ASSERT_EQUAL_PTR(&node3, list.head.next);
    TEST_ASSERT_NULL(node1.next);

    /* remove last node */
    TEST_ASSERT_EQUAL(MLG_OK, mlg_slist_remove(&list, &node3));
    TEST_ASSERT_EQUAL_PTR(&list.head, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, list.tail);
    TEST_ASSERT_NULL(node3.next);
}

void test_slist_iteration_macros(void)
{
    mlg_slist_t list;
    mlg_slist_init(&list);

    mlg_slist_node_t nodes[3];
    for (int i = 0; i < 3; i++)
    {
        mlg_slist_push_back(&list, &nodes[i]);
    }

    mlg_slist_node_t *pos;
    int count = 0;
    mlg_slist_for_each(pos, &list)
    {
        TEST_ASSERT_EQUAL_PTR(&nodes[count], pos);
        count++;
    }
    TEST_ASSERT_EQUAL(3, count);

    mlg_slist_node_t *n;
    count = 0;
    mlg_slist_for_each_safe(pos, n, &list)
    {
        TEST_ASSERT_EQUAL_PTR(&nodes[count], pos);
        mlg_slist_remove(&list, pos);
        count++;
    }
    TEST_ASSERT_EQUAL(3, count);
    TEST_ASSERT_EQUAL_PTR(&list.head, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&list.head, list.tail);
}

int main(void)
{
    UnityBegin("test_intrusive_slist.c");
    RUN_TEST(test_slist_init);
    /* insertion tests */
    RUN_TEST(test_slist_push_back_single);
    RUN_TEST(test_slist_push_back_multiple);
    RUN_TEST(test_slist_push_front_single);
    RUN_TEST(test_slist_push_front_multiple);
    RUN_TEST(test_slist_insert_after);
    RUN_TEST(test_slist_insert_before);
    /* insertion boundary cases */
    RUN_TEST(test_slist_insert_after_at_tail);
    RUN_TEST(test_slist_insert_before_at_head);
    RUN_TEST(test_slist_insert_after_sentinel_matches_push_front);
    RUN_TEST(test_slist_insert_before_sentinel_matches_push_back);
    RUN_TEST(test_slist_insert_after_single_element_list);
    RUN_TEST(test_slist_insert_before_single_element_list);
    /* deletion tests */
    RUN_TEST(test_slist_remove_multiple);
    /* iteration tests */
    RUN_TEST(test_slist_iteration_macros);

    return UnityEnd();
}
