#include "stdlib.h"
#include "unistd.h"
#include "../unit_test.h"

#define SMALL_SLEEP (usleep(200000))

UNIT_TEST(immediate_fail) {
    SMALL_SLEEP;

    UNIT_TEST_FAIL("Failed correctly.");
}

UNIT_TEST(assert_null) {
    SMALL_SLEEP;

    void* c = malloc(1);
    ASSERT_NULL(c, "Non-null pointer is not null.");
    UNIT_TEST_END;
}

UNIT_TEST(assert_not_null) {
    SMALL_SLEEP;

    ASSERT_NOT_NULL(NULL, "Null pointer is null.");
    UNIT_TEST_END;
}

UNIT_TEST(assert_true) {
    SMALL_SLEEP;

    ASSERT_TRUE(0 == 1, "0 is not equal to 1");
    UNIT_TEST_END;
}

UNIT_TEST(assert_false) {
    SMALL_SLEEP;

    ASSERT_FALSE(0 == 0, "0 is equal 0.");
    UNIT_TEST_END;
}

UNIT_TEST(assert_equal) {
    SMALL_SLEEP;
    
    ASSERT_EQUAL(0, 1, "0 is not equal to 1.");
    UNIT_TEST_END;
}

UNIT_TEST(assert_not_equal) {
    SMALL_SLEEP;

    ASSERT_NOT_EQUAL(0, 0, "0 is equal to 0.");
    UNIT_TEST_END;
}

UNIT_TESTS(
    immediate_fail,
    assert_null,
    assert_not_null,
    assert_true,
    assert_false,
    assert_equal,
    assert_not_equal
)

