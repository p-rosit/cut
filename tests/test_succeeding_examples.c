#include "stdlib.h"
#include "unistd.h"
#include "../cut.h"

#define SMALL_SLEEP (usleep(100000))

/* Function protoype */
UNIT_TEST(immediate_success);

/* Implementation of test */
UNIT_TEST(immediate_success) {
    SMALL_SLEEP;

    TEST_END;
}

UNIT_TEST(assert_null) {
    SMALL_SLEEP;

    ASSERT_NULL(NULL, "Null pointer is not equal to null.");
    TEST_END;
}

UNIT_TEST(assert_not_null) {
    SMALL_SLEEP;

    void* c = malloc(1);
    free(c);
    ASSERT_NOT_NULL(c, "Non null pointer is null.");
    TEST_END;
}

UNIT_TEST(assert_true) {
    SMALL_SLEEP;

    ASSERT_TRUE(0 == 0, "0 is not equal to 0.");
    TEST_END;
}

UNIT_TEST(assert_false) {
    SMALL_SLEEP;

    ASSERT_FALSE(0 == 1, "0 is equal to 1.");
    TEST_END;
}

UNIT_TEST(assert_equal) {
    SMALL_SLEEP;
    
    ASSERT_EQUAL(0, 0, "0 is not equal to 0.");
    TEST_END;
}

UNIT_TEST(assert_not_equal) {
    SMALL_SLEEP;

    ASSERT_NOT_EQUAL(0, 1, "0 is equal to 1.");
    TEST_END;
}

LIST_TESTS(
    immediate_success,
    assert_null,
    assert_not_null,
    assert_true,
    assert_false,
    assert_equal,
    assert_not_equal,
)

