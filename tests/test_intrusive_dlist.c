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

void test_dlist_push_back(void)
{
    mlg_dlist_t list;
    mlg_dlist_init(&list);

    mlg_dlist_node_t node1;
    mlg_error_t err = mlg_dlist_push_back(&list, &node1);
    TEST_ASSERT_EQUAL(MLG_OK, err);
    TEST_ASSERT_EQUAL_PTR(&node1, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node1, list.head.prev);
}

void test_dlist_push_front(void)
{
    mlg_dlist_t list;
    mlg_dlist_init(&list);

    mlg_dlist_node_t node1;
    mlg_error_t err = mlg_dlist_push_front(&list, &node1);
    TEST_ASSERT_EQUAL(MLG_OK, err);
    TEST_ASSERT_EQUAL_PTR(&node1, list.head.next);
    TEST_ASSERT_EQUAL_PTR(&node1, list.head.prev);
}

void test_dlist_remove(void)
{
    mlg_dlist_t list;
    mlg_dlist_init(&list);

    mlg_dlist_node_t node1;
    mlg_error_t err = mlg_dlist_push_front(&list, &node1);
    TEST_ASSERT_EQUAL(MLG_OK, err);

}

int main(void)
{
    UnityBegin("test_intrusive_dlist.c");
    RUN_TEST(test_dlist_init);
    /* insertion tests */
    RUN_TEST(test_dlist_push_back);
    RUN_TEST(test_dlist_push_front);
    /* deletion tests */

    return UnityEnd();
}
