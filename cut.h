#ifndef CUT_H
#define CUT_H

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>

#define CUT_MESSAGE_SIZE          (2048)
#define CUT_BAR_SIZE              (512)
#define CUT_FUNCTION_NAME_SIZE    (20)
#define CUT_LINE_NUMBER_SIZE      (3)

#define TEST_END \
    do {return CUT_SUCCESS;} while (0)
#define TEST_BROKEN \
    do {CUT_MAKE_MESSAGE("[Test marked broken]"); cutp_broken_flag = 1; return CUT_FAIL;} while (0)
#define TEST_FAIL(...) \
    do {CUT_MAKE_MESSAGE(__VA_ARGS__); return CUT_FAIL;} while(0)
#define CUT_BOOLEAN_CHECK(expression, ...) \
    do {if (!(expression)) {TEST_FAIL(__VA_ARGS__);}} while(0)
#define CUT_MAKE_MESSAGE(...) \
    do {cutp_make_return_message_var_args(CUT_DEBUG_INFO, __VA_ARGS__);} while(0)

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
    ((cutp_debug_information_t) {                                               \
        .function_name = __func__,                                              \
        .line_number = __LINE__                                                 \
     })

#ifndef CUT_FILE_BROKEN
#define LIST_TESTS(...)                                                         \
    int main() {                                                                \
        int total_tests, total_succeeded;                                       \
        cutp_test_information_t info;                                           \
                                                                                \
        cutp_func fs[] = {__VA_ARGS__};                                         \
        total_tests = sizeof fs / sizeof(cutp_func);                            \
                                                                                \
        info.total_tests = total_tests;                                         \
        info.results = malloc(total_tests * sizeof(int));                       \
                                                                                \
        total_succeeded = cutp_test_all_functions(&info, fs);                   \
        printf("\n");                                                           \
                                                                                \
        free(info.results);                                                     \
        return total_succeeded;                                                 \
    }
#else
#define LIST_TESTS(...)                                                         \
    int main() {                                                                \
        int total_tests;                                                        \
        cutp_test_information_t info;                                           \
                                                                                \
        cutp_func fs[] = {__VA_ARGS__};                                         \
        total_tests = sizeof fs / sizeof(cutp_func);                            \
                                                                                \
        info.total_tests = total_tests;                                         \
        info.results = malloc(total_tests * sizeof(int));                       \
                                                                                \
        cutp_prepare_tests(&info);                                              \
        cutp_finish_tests(&info);                                               \
        printf(" [File marked broken]\n");                                      \
                                                                                \
        free(info.results);                                                     \
        return -1;                                                              \
    }
#endif

#define CUT_FAIL    (0)
#define CUT_SUCCESS (1)
#define CUT_BROKEN  (2)
#define CUT_STARTED (3)
#define CUT_NO_TEST (4)

#define CUT_COL_RED     "\x1b[31m"
#define CUT_COL_GREEN   "\x1b[32m"
#define CUT_COL_YELLOW  "\x1b[33m"
#define CUT_COL_BLUE    "\x1b[34m"
#define CUT_COL_MAGENTA "\x1b[35m"
#define CUT_COL_CYAN    "\x1b[36m"
#define CUT_COL_RESET   "\x1b[0m"

char        cutp_message[CUT_MESSAGE_SIZE];
char        cutp_broken_flag = 0;
const char  test_prefix[] = "Running tests:";
const char  cutp_start_symbol[]    = ">";
const char  cutp_success_symbol[]  = "·";
const char  cutp_fail_symbol[]     = "x";
const char  cutp_broken_symbol[]   = " ";

typedef int (*cutp_func)();
#define UNIT_TEST(function)         int function()
#define SUB_TEST(function, ...)     int function(__VA_ARGS__)

typedef struct cutp_test_information    cutp_test_information_t;
typedef struct cutp_test_result         cutp_test_result_t;
typedef struct cutp_debug_information   cutp_debug_information_t;

struct cutp_test_result {
    int index;
    int status;
    int test_broken;
};

struct cutp_test_information {
    int total_tests;
    int succeeded_tests;
    int* results;
    cutp_test_result_t test_info;
};

struct cutp_debug_information {
    const char *function_name;
    int line_number;
};

int     cutp_test_all_functions(cutp_test_information_t*, cutp_func*);
int     cutp_test_function(cutp_test_information_t*, cutp_func);
void    cutp_prepare_tests(cutp_test_information_t*);
void    cutp_finish_tests(cutp_test_information_t*);
void    cutp_start_test(cutp_test_information_t*);
void    cutp_end_test(cutp_test_information_t*, int);
void    cutp_print_finish_prefix(cutp_test_information_t*);
void    cutp_print_test_prefix();

void    cutp_report_error();
void    cutp_make_return_message_var_args(cutp_debug_information_t, char*, ...);
void    cutp_make_return_message(cutp_debug_information_t, char*, va_list);
void    cutp_error_format_string(char*, cutp_debug_information_t, char*);

void    cutp_make_bar(cutp_test_information_t*);
void    cutp_write_to_bar(cutp_test_information_t*, int);
void    cutp_print_bar(cutp_test_information_t*);
void    cutp_clear_bar(cutp_test_information_t*);


int cutp_test_all_functions(cutp_test_information_t* info, cutp_func* funcs) {
    int status;
    cutp_prepare_tests(info);

    for (int index = 0; index < info->total_tests; index++) {
        info->test_info.index = index;
        status = cutp_test_function(info, funcs[index]);

#ifndef CUT_CONTINUE_ON_FAIL
        if (status == CUT_FAIL && !info->test_info.test_broken) {
            break;
        }
#endif

        info->succeeded_tests += status;
    }

    cutp_finish_tests(info);
    return info->succeeded_tests;
}

void cutp_prepare_tests(cutp_test_information_t* info) {
    info->succeeded_tests = 0;

    cutp_make_bar(info);
    cutp_print_test_prefix();
    cutp_print_bar(info);
}

void cutp_finish_tests(cutp_test_information_t* info) {
    cutp_clear_bar(info);
    cutp_print_finish_prefix(info);
    cutp_print_bar(info);
}

void cutp_print_finish_prefix(cutp_test_information_t* info) {
    printf("\r[%d/%d] <= ", info->succeeded_tests, info->total_tests);
}

int cutp_test_function(cutp_test_information_t* info, cutp_func func) {
    int status;
    cutp_start_test(info);

    status = (*func)();

    cutp_clear_bar(info);
    cutp_end_test(info, status);

    return status;
}

void cutp_start_test(cutp_test_information_t* info) {
    info->test_info.test_broken = 0;
    cutp_broken_flag = 0;

    cutp_write_to_bar(info, CUT_STARTED);
    cutp_print_test_prefix();
    cutp_print_bar(info);
}

void cutp_end_test(cutp_test_information_t* info, int status) {
    if (cutp_broken_flag) {
        cutp_broken_flag = 0;
        info->test_info.test_broken = 1;
        status = CUT_BROKEN;
    }

    if (status == CUT_FAIL || status == CUT_BROKEN) {
        cutp_report_error();
    }

    cutp_write_to_bar(info, status);
    cutp_print_test_prefix();
    cutp_print_bar(info);
}

void cutp_report_error() {
    printf("%s\n", cutp_message);
}

void cutp_make_bar(cutp_test_information_t* info) {
    for (int i = 0; i < info->total_tests; i++)
        info->results[i] = CUT_NO_TEST;
}

void cutp_write_to_bar(cutp_test_information_t* info, int status) {
    info->results[info->test_info.index] = status;
}

void cutp_print_test_prefix() {
    printf("\r%s: ", test_prefix);
}

void cutp_print_bar(cutp_test_information_t* info) {
    printf("[");
   
    for (int i = 0; i < info->total_tests; i++) {
        switch (info->results[i]) {
            case CUT_STARTED:
                printf("%s", cutp_start_symbol);
                break;
            case CUT_SUCCESS:
                printf("%s", cutp_success_symbol);
                break;
            case CUT_FAIL:
                printf(CUT_COL_RED "%s" CUT_COL_RESET, cutp_fail_symbol);
                break;
            case CUT_BROKEN:
                printf("%s", cutp_broken_symbol);
                break;
            case CUT_NO_TEST:
                printf(" ");
                break;
        }
    }

    printf("]");
    fflush(stdout);
}

void cutp_clear_bar(cutp_test_information_t* info) {
    int space = 1;
    int bar_enclosure = 2;
    int clear_length = (sizeof test_prefix) + space + bar_enclosure + info->total_tests;

    printf("\r%*c\r", clear_length, ' ');
    fflush(stdout);
}

void cutp_make_return_message_var_args(cutp_debug_information_t info, char* format, ...) {
    va_list valist;
    if (format[0] == '\0') return;

    va_start(valist, format);
    cutp_make_return_message(info, format, valist);
    va_end(valist);
}

void cutp_make_return_message(cutp_debug_information_t info, char *format, va_list valist) {
    char new_format[CUT_MESSAGE_SIZE];
    cutp_error_format_string(new_format, info, format);
    vsprintf(cutp_message, new_format, valist);
}

void cutp_error_format_string(char* format, cutp_debug_information_t info, char* old_format) {
    sprintf(
        format, (CUT_COL_RED "[%*s, line %*d] %s" CUT_COL_RESET),
        CUT_FUNCTION_NAME_SIZE, info.function_name,
        CUT_LINE_NUMBER_SIZE, info.line_number,
        old_format
    );
}

#endif

