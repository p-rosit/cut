#define CUT_FILE_BROKEN
#include "../cut.h"

UNIT_TEST(test_that_is_broken) {
    ASSERT_TRUE(0 == 1, "Test should not run.");
    TEST_END;
}

LIST_TESTS(
    test_that_is_broken,
    test_that_is_broken,
    test_that_is_broken,
    test_that_is_broken,
)

