#include "stdlib.h"
#include "unistd.h"
#include "../../unit_test.h"

#define SMALL_SLEEP (usleep(200000))

UNIT_TEST(immediate_success) {
    SMALL_SLEEP;

    UNIT_TEST_END;
}

UNIT_TEST(assert_null) {
    SMALL_SLEEP;

    ASSERT_NULL(NULL, "Null pointer is not equal to null.\n");
    UNIT_TEST_END;
}

UNIT_TEST(assert_not_null) {
    SMALL_SLEEP;

    void* c = malloc(1);
    ASSERT_NOT_NULL(c, "Non null pointer is null.\n");
    UNIT_TEST_END;
}

UNIT_TEST(assert_true) {
    SMALL_SLEEP;

    ASSERT_TRUE(0 == 0, "0 is not equal to 0.\n");
    UNIT_TEST_END;
}

UNIT_TEST(assert_false) {
    SMALL_SLEEP;

    ASSERT_FALSE(0 == 1, "0 is equal to 1.\n");
    UNIT_TEST_END;
}

UNIT_TEST(assert_equal) {
    SMALL_SLEEP;
    
    ASSERT_EQUAL(0, 0, "0 is not equal to 0.\n");
    UNIT_TEST_END;
}

UNIT_TEST(assert_not_equal) {
    SMALL_SLEEP;

    ASSERT_NOT_EQUAL(0, 1, "0 is equal to 1.\n");
    UNIT_TEST_END;
}

UNIT_TEST(immediate_fail) {
    SMALL_SLEEP;

    UNIT_TEST_FAIL("Test failed successfully.\n");
    UNIT_TEST_END;
}

UNIT_TEST(incorrect_assert_null) {
    SMALL_SLEEP;

    void* c = malloc(1);
    ASSERT_NULL(c, "Assert null fails on non-null pointer.\n");
    UNIT_TEST_END;
}

UNIT_TEST(are_not_equal) {
    SMALL_SLEEP;

    ASSERT_EQUAL(5, 6, "Expected a and b to be equal a == %d, b == %d\n", 5, 6);
    UNIT_TEST_END;
}

UNIT_TESTS(
    immediate_success,
    assert_null,
    assert_not_null,
    assert_true,
    assert_false,
    assert_equal,
    assert_not_equal
)

