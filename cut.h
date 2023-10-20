#ifndef CUT_H
#define CUT_H

#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#define CUT_START_SYMBOL          ('.')
#define CUT_SUCCESS_SYMBOL        ('-')
#define CUT_FAIL_SYMBOL           ('x')
#define CUT_BROKEN_SYMBOL         (' ')

#define CUT_MESSAGE_SIZE          (2048)
#define CUT_BAR_SIZE              (512)
#define CUT_FUNCTION_NAME_SIZE    (20)
#define CUT_LINE_NUMBER_SIZE      (3)

#define TEST_END \
    do {return CUT_SUCCESS;} while (0)
#define TEST_BROKEN \
    do {cutp_make_return_message_var_args(CUT_DEBUG_INFO, "[Test marked broken]"); cutp_broken_flag = 1; return CUT_FAIL;} while (0)
#define TEST_FAIL(...) \
    do {cutp_make_return_message_var_args(CUT_DEBUG_INFO, __VA_ARGS__); return CUT_FAIL;} while(0)
#define CUT_BOOLEAN_CHECK(expression, ...) \
    do {if (!(expression)) {TEST_FAIL(__VA_ARGS__);}} while(0)

#define ASSERT_FALSE(bool, ...) \
    CUT_BOOLEAN_CHECK((bool) == 0, __VA_ARGS__)
#define ASSERT_TRUE(bool, ...) \
    CUT_BOOLEAN_CHECK((bool) != 0, __VA_ARGS__)

#define ASSERT_NULL(ptr, ...) \
    CUT_BOOLEAN_CHECK((ptr) == NULL, __VA_ARGS__)
#define ASSERT_NOT_NULL(ptr, ...) \
    CUT_BOOLEAN_CHECK((ptr) != NULL, __VA_ARGS__)

#define ASSERT_EQUAL(val1, val2, ...) \
    CUT_BOOLEAN_CHECK((val1) == (val2), __VA_ARGS__)
#define ASSERT_NOT_EQUAL(val1, val2, ...) \
    CUT_BOOLEAN_CHECK((val1) != (val2), __VA_ARGS__)

#define CUT_DEBUG_INFO                                                          \
    ((cut_debug_information_t) {                                                \
        .function_name = __func__,                                              \
        .line_number = __LINE__                                                 \
     })
#define CUT_FAIL    (0)
#define CUT_SUCCESS (1)

#ifndef CUT_FILE_BROKEN
#define LIST_TESTS(...)                                                         \
    int main() {                                                                \
        int total_tests, total_succeeded;                                       \
                                                                                \
        cutp_func fs[] = {__VA_ARGS__};                                         \
        total_tests = sizeof fs / sizeof(cutp_func);                            \
                                                                                \
        total_succeeded = cutp_test_all_functions(total_tests, fs);             \
        printf("\n");                                                           \
                                                                                \
        return total_succeeded;                                                 \
    }
#else
#define LIST_TESTS(...)                                                         \
    int main() {                                                                \
        int total_tests;                                                        \
                                                                                \
        cutp_func fs[] = {__VA_ARGS__};                                         \
        total_tests = sizeof fs / sizeof(cutp_func);                            \
                                                                                \
        cutp_prepare_tests(total_tests);                                        \
        cutp_finish_tests(0, total_tests);                                      \
        printf(" [File marked broken]\n");                                      \
                                                                                \
        return -1;                                                              \
    }
#endif

#define CUT_COL_RED     "\x1b[31m"
#define CUT_COL_GREEN   "\x1b[32m"
#define CUT_COL_YELLOW  "\x1b[33m"
#define CUT_COL_BLUE    "\x1b[34m"
#define CUT_COL_MAGENTA "\x1b[35m"
#define CUT_COL_CYAN    "\x1b[36m"
#define CUT_COL_RESET   "\x1b[0m"


char test_prefix[] = "Running tests:";
char cutp_message[CUT_MESSAGE_SIZE];
char cutp_bar[CUT_BAR_SIZE];
char cutp_broken_flag = 0;

typedef int (*cutp_func)();
#define UNIT_TEST(function) int function()
#define SUB_TEST(function, ...)  int function(__VA_ARGS__)

typedef struct cut_debug_information {
    const char *function_name;
    int line_number;
} cut_debug_information_t;

int     cutp_test_all_functions(int, cutp_func*);
int     cutp_test_function(cutp_func, int, int);
void    cutp_prepare_tests(int);
void    cutp_finish_tests(int, int);
void    cutp_start_test(int);
void    cutp_end_test(int, int);
void    cutp_print_finish_prefix(int, int);
void    cutp_print_test_prefix();

char    cutp_status_to_symbol(int);
void    cutp_report_error();
void    cutp_make_return_message_var_args(cut_debug_information_t, char*, ...);
void    cutp_make_return_message(cut_debug_information_t, char*, va_list);
void    cutp_error_format_string(char*, cut_debug_information_t, char*);

void    cutp_make_bar(int);
void    cutp_write_to_bar(int, char);
void    cutp_print_bar();
void    cutp_clear_bar(int);


int cutp_test_all_functions(int total_tests, cutp_func* funcs) {
    int index, status, total_succeeded;

    cutp_prepare_tests(total_tests);

    for (index = 0, total_succeeded = 0; index < total_tests; index++) {
        status = cutp_test_function(funcs[index], index, total_tests);

#ifndef CUT_CONTINUE_ON_FAIL
        if (status == CUT_FAIL && !cutp_broken_flag) {
            break;
        }
#endif

        cutp_broken_flag = 0;
        total_succeeded += status == CUT_SUCCESS;
    }

    cutp_finish_tests(total_succeeded, total_tests);
    return total_succeeded;
}

void cutp_prepare_tests(int total_test) {
    cutp_make_bar(total_test);
    cutp_print_test_prefix();
    cutp_print_bar();
}

void cutp_finish_tests(int total_succeeded, int total_tests) {
    cutp_clear_bar(total_tests);
    cutp_print_finish_prefix(total_succeeded, total_tests);
    cutp_print_bar();
}

void cutp_print_finish_prefix(int total_succeeded, int total_tests) {
    printf("\r[%d/%d] <= ", total_succeeded, total_tests);
}

int cutp_test_function(cutp_func func, int index, int total_tests) {
    int status;
    cutp_start_test(index);

    status = (*func)();

    cutp_clear_bar(total_tests);
    cutp_end_test(index, status);

    return status;
}

void cutp_start_test(int index) {
    cutp_write_to_bar(index, CUT_START_SYMBOL);
    cutp_print_test_prefix();
    cutp_print_bar();
}

void cutp_end_test(int index, int status) {
    char print_symbol;
    
    if (status == CUT_FAIL) {
        cutp_report_error();
    }

    print_symbol = cutp_status_to_symbol(status);
    cutp_write_to_bar(index, print_symbol);
    cutp_print_test_prefix();
    cutp_print_bar();
}

char cutp_status_to_symbol(int status) {
    if (status) {
        return CUT_SUCCESS_SYMBOL;
    } else {
        return !cutp_broken_flag * CUT_FAIL_SYMBOL + cutp_broken_flag * CUT_BROKEN_SYMBOL;
    }
}

void cutp_report_error() {
    printf("%s\n", cutp_message);
}

void cutp_make_bar(int length) {
    for (int i = 0; i < length; i++)
        cutp_bar[i] = ' ';
    cutp_bar[length + 1] = '\0';
}

void cutp_write_to_bar(int index, char c) {
    cutp_bar[index] = c;
}

void cutp_print_test_prefix() {
    printf("\r%s: ", test_prefix);
}

void cutp_print_bar() {
    printf("[");
    
    char *bar, c;
    for (bar = cutp_bar, c = *bar++; c != '\0'; c = *bar++) {
        switch (c) {
            case CUT_START_SYMBOL:
            case CUT_SUCCESS_SYMBOL:
                printf("%c", c);
                break;
            case CUT_BROKEN_SYMBOL:
            case CUT_FAIL_SYMBOL:
                printf(CUT_COL_RED "%c" CUT_COL_RESET, c);
                break;
        }
    }

    printf("]");
    fflush(stdout);
}

void cutp_clear_bar(int bar_length) {
    int space = 1;
    int bar_enclosure = 2;
    int clear_length = (sizeof test_prefix) + space + bar_enclosure + bar_length;

    printf("\r%*c\r", clear_length, ' ');
    fflush(stdout);
}

void cutp_make_return_message_var_args(cut_debug_information_t info, char* format, ...) {
    va_list valist;
    if (format[0] == '\0') return;

    va_start(valist, format);
    cutp_make_return_message(info, format, valist);
    va_end(valist);
}

void cutp_make_return_message(cut_debug_information_t info, char *format, va_list valist) {
    char new_format[CUT_MESSAGE_SIZE];
    cutp_error_format_string(new_format, info, format);
    vsprintf(cutp_message, new_format, valist);
}

void cutp_error_format_string(char* format, cut_debug_information_t info, char* old_format) {
    sprintf(
        format, (CUT_COL_RED "[%*s, line %*d] %s" CUT_COL_RESET),
        CUT_FUNCTION_NAME_SIZE, info.function_name,
        CUT_LINE_NUMBER_SIZE, info.line_number,
        old_format
    );
}

#endif

