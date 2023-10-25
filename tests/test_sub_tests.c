#include <string.h>
#include "../cut.h"

UNIT_TEST(call_one);

UNIT_TEST(call_two);
SUB_TEST(middle_call);

UNIT_TEST(call_three);
SUB_TEST(call_first);
SUB_TEST(call_second);

UNIT_TEST(pop_test);
SUB_TEST(first_pop);
SUB_TEST(second_pop);
SUB_TEST(third_pop);

SUB_TEST(check_call_stack, char* expected);

UNIT_TEST(call_one) {
    CALL_TEST(check_call_stack, "call_one");
    TEST_END;
}

UNIT_TEST(call_two) {
    CALL_TEST(check_call_stack, "call_two");
    CALL_TEST(middle_call);
    TEST_END;
}

SUB_TEST(middle_call) {
    CALL_TEST(check_call_stack, "call_two.middle_call");
    TEST_END;
}

UNIT_TEST(call_three) {
    CALL_TEST(check_call_stack, "call_three");
    CALL_TEST(call_first);
    TEST_END;
}

SUB_TEST(call_first) {
    CALL_TEST(check_call_stack, "call_three.call_first");
    CALL_TEST(call_second);
    TEST_END;
}

SUB_TEST(call_second) {
    CALL_TEST(check_call_stack, "call_three.call_first.call_second");
    TEST_END;
}

UNIT_TEST(pop_test) {
    CALL_TEST(check_call_stack, "pop_test");
    CALL_TEST(first_pop);
    CALL_TEST(check_call_stack, "pop_test");
    CALL_TEST(second_pop);
    CALL_TEST(check_call_stack, "pop_test");
    CALL_TEST(third_pop);
    CALL_TEST(check_call_stack, "pop_test");
    TEST_END;
}

SUB_TEST(first_pop) {
    CALL_TEST(check_call_stack, "pop_test.first_pop");
    TEST_END;
}

SUB_TEST(second_pop) {
    CALL_TEST(check_call_stack, "pop_test.second_pop");
    TEST_END;
}

SUB_TEST(third_pop) {
    CALL_TEST(check_call_stack, "pop_test.third_pop");
    TEST_END;
}

SUB_TEST(check_call_stack, char* expected) {
    /*
     * This function is verifying that the internals of the testing is
     * working as intended. the cutp_test_info struct should never be
     * accessed by a normal test.
     */
    char* call_stack = malloc(strlen(cutp_test_info->test_result.call_stack) + 1);
    strcpy(call_stack, cutp_test_info->test_result.call_stack);

    int eq = strcmp(expected, call_stack) == 0;
    ASSERT_TRUE(eq, "Expected \"%s\" got \"%s\".", expected, call_stack);

    free(call_stack);
    TEST_END;
}

LIST_TESTS(
    call_one,
    call_two,
    call_three,
    pop_test,
)

