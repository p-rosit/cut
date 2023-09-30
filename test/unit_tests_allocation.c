#include <stdlib.h>
#include "../unit_test.h"

UNIT_TEST(is_null) {
    void* ptr = NULL;

    ASSERT_NULL(ptr, "Pointer (%p) is not null.", ptr);

    UNIT_TEST_END;
}

UNIT_TEST(allocation_not_null) {
    void* ptr = NULL;

    ptr = malloc(1);
    ASSERT_NOT_NULL(ptr, "Pointer is null.");

    UNIT_TEST_END;
}

UNIT_TESTS(
    is_null,
    allocation_not_null,
    is_null,
    is_null,
    allocation_not_null,
    is_null,
    is_null,
    is_null,
    allocation_not_null,
    is_null,
    is_null,
    is_null,
    allocation_not_null
)
