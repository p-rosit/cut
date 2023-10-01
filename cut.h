#ifndef UNIT_TEST_H
#define UNIT_TEST_H

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#define UNIT_TEST_START_SYMBOL          ('.')
#define UNIT_TEST_SUCCESS_SYMBOL        ('-')
#define UNIT_TEST_FAIL_SYMBOL           ('x')

#define UNIT_TEST_MESSAGE_SIZE          (2048)
#define UNIT_TEST_BAR_SIZE              (256)
#define UNIT_TEST_FUNCTION_NAME_SIZE    (20)
#define UNIT_TEST_LINE_NUMBER_SIZE      (3)

#define UNIT_TEST_END \
    do {return 1;} while (0)
#define UNIT_TEST_BROKEN \
    do {return 1;} while (0)
#define UNIT_TEST_FAIL(...) \
    do {unit_test_p_make_return_message((debug_information_t) {.function_name = __func__, .line_number = __LINE__}, __VA_ARGS__); return 0;} while(0)
#define UNIT_TEST_BOOLEAN_CHECK(expression, ...) \
    do {if (!(expression)) {UNIT_TEST_FAIL(__VA_ARGS__);}} while(0)

#define ASSERT_FALSE(bool, ...) \
    UNIT_TEST_BOOLEAN_CHECK((bool) == 0, __VA_ARGS__)
#define ASSERT_TRUE(bool, ...) \
    UNIT_TEST_BOOLEAN_CHECK((bool) != 0, __VA_ARGS__)

#define ASSERT_NULL(ptr, ...) \
    UNIT_TEST_BOOLEAN_CHECK((ptr) == NULL, __VA_ARGS__)
#define ASSERT_NOT_NULL(ptr, ...) \
    UNIT_TEST_BOOLEAN_CHECK((ptr) != NULL, __VA_ARGS__)

#define ASSERT_EQUAL(val1, val2, ...) \
    UNIT_TEST_BOOLEAN_CHECK((val1) == (val2), __VA_ARGS__)
#define ASSERT_NOT_EQUAL(val1, val2, ...) \
    UNIT_TEST_BOOLEAN_CHECK((val1) != (val2), __VA_ARGS__)

#ifndef UNIT_TEST_FILE_BROKEN
#define UNIT_TESTS(...) \
    int main() {\
        unit_test_p_func fs[] = {__VA_ARGS__};\
        int total_tests = sizeof fs / sizeof(unit_test_p_func);\
        unit_test_p_test_all_functions(total_tests, fs);\
        printf("\n");\
        return 0;\
    }
#else
#define UNIT_TESTS(...) \
    int main() {\
        unit_test_p_func fs[] = {__VA_ARGS__};\
        int total_tests = sizeof fs / sizeof(unit_test_p_func);\
        unit_test_p_prepare_tests(total_tests);\
        unit_test_p_finish_tests(0, total_tests);\
        printf(" [File marked broken]\n");\
        return 0;\
    }
#endif

char test_prefix[] = "Running tests:";
char unit_test_p_message[UNIT_TEST_MESSAGE_SIZE];
char unit_test_p_bar[UNIT_TEST_BAR_SIZE];

typedef int (*unit_test_p_func)();
#define UNIT_TEST(function) int function()

typedef struct debug_information {
    const char *function_name;
    int line_number;
} debug_information_t;

void    unit_test_p_test_all_functions(int, unit_test_p_func*);
int     unit_test_p_test_function(unit_test_p_func, int, int);
void    unit_test_p_prepare_tests(int);
void    unit_test_p_finish_tests(int, int);
void    unit_test_p_start_test(int);
void    unit_test_p_end_test(int, int);

void    unit_test_p_report_error();
void    unit_test_p_make_return_message(debug_information_t, char*, ...);
void    unit_test_p_error_format_string(char*, debug_information_t, char*);

void    unit_test_p_make_bar(int);
void    unit_test_p_write_to_bar(int, char);
void    unit_test_p_print_bar();
void    unit_test_p_clear_bar(int);


void unit_test_p_test_all_functions(int total_tests, unit_test_p_func* funcs) {
    int index, succeeded, total_succeeded;

    unit_test_p_prepare_tests(total_tests);

    for (index = 0, total_succeeded = 0; index < total_tests; index++) {
        succeeded = unit_test_p_test_function(funcs[index], index, total_tests);

#ifndef UNIT_TEST_CONTINUE_ON_FAIL
        if (!succeeded) {
            break;
        }
#endif

        total_succeeded += succeeded;
    }

    unit_test_p_finish_tests(total_succeeded, total_tests);
}

void unit_test_p_prepare_tests(int total_test) {
    unit_test_p_make_bar(total_test);
    unit_test_p_print_bar();
}

void unit_test_p_finish_tests(int total_succeeded, int total_tests) {
    unit_test_p_clear_bar(total_tests);
    printf("\r[%s] => [%d/%d]", unit_test_p_bar, total_succeeded, total_tests);
}

int unit_test_p_test_function(unit_test_p_func func, int index, int total_tests) {
    int succeeded;
    unit_test_p_start_test(index);

    succeeded = (*func)();

    unit_test_p_clear_bar(total_tests);
    unit_test_p_end_test(index, succeeded);

    return succeeded;
}

void unit_test_p_start_test(int index) {
    unit_test_p_write_to_bar(index, UNIT_TEST_START_SYMBOL);
    unit_test_p_print_bar();
}

void unit_test_p_end_test(int index, int succeeded) {
    if (succeeded) {
        unit_test_p_write_to_bar(index, UNIT_TEST_SUCCESS_SYMBOL);
    } else {
        unit_test_p_report_error();
        unit_test_p_write_to_bar(index, UNIT_TEST_FAIL_SYMBOL);
    }
    unit_test_p_print_bar();
}

void unit_test_p_report_error() {
    printf("%s\n", unit_test_p_message);
}

void unit_test_p_make_bar(int length) {
    for (int i = 0; i < length; i++)
        unit_test_p_bar[i] = ' ';
    unit_test_p_bar[length + 1] = '\0';
}

void unit_test_p_write_to_bar(int index, char c) {
    unit_test_p_bar[index] = c;
}

void unit_test_p_print_bar() {
    printf("\r%s [%s]", test_prefix, unit_test_p_bar);
    fflush(stdout);
}

void unit_test_p_clear_bar(int bar_length) {
    int space = 1;
    int bar_enclosure = 2;
    int clear_length = (sizeof test_prefix) + space + bar_enclosure + bar_length;
    printf("\r%*c\r", clear_length, ' ');
    fflush(stdout);
}

void unit_test_p_make_return_message(debug_information_t info, char *format, ...) {
    char new_format[UNIT_TEST_MESSAGE_SIZE];
    va_list valist;
  
    unit_test_p_error_format_string(new_format, info, format);

    va_start(valist, format);
    vsprintf(unit_test_p_message, new_format, valist);
    va_end(valist);
}

void unit_test_p_error_format_string(char* format, debug_information_t info, char* old_format) {
    sprintf(
        format, "[%*s, line %*d] %s",
        UNIT_TEST_FUNCTION_NAME_SIZE, info.function_name,
        UNIT_TEST_LINE_NUMBER_SIZE, info.line_number,
        old_format
    );
}

#endif

