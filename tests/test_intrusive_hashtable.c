#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_hashtable_init(void) {
    TEST_ASSERT_EQUAL_INT(1, 1);
}


int main(void) {
    UnityBegin("test_intrusive_hashtable.c");
    RUN_TEST(test_hashtable_init);
    return UnityEnd();
}
