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
    do {cutp_make_return_message_var_args(CUT_DEBUG_INFO, "[Test marked broken]"); return CUT_BROKEN;} while (0)
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

#define CUT_DEBUG_INFO \
    ((cut_debug_information_t) {\
        .function_name = __func__,\
        .line_number = __LINE__\
     })
#define CUT_FAIL    (0)
#define CUT_SUCCESS (1)
#define CUT_BROKEN  (2)

#ifndef CUT_FILE_BROKEN
#define LIST_TESTS(...) \
    int main() {\
        int total_tests, total_succeeded;\
        \
        cutp_func fs[] = {__VA_ARGS__};\
        total_tests = sizeof fs / sizeof(cutp_func);\
        \
        total_succeeded = cutp_test_all_functions(total_tests, fs);\
        printf("\n");\
        \
        return total_succeeded;\
    }
#else
#define LIST_TESTS(...) \
    int main() {\
        int total_tests;\
        \
        cutp_func fs[] = {__VA_ARGS__};\
        total_tests = sizeof fs / sizeof(cutp_func);\
        \
        cutp_prepare_tests(total_tests);\
        cutp_finish_tests(0, total_tests);\
        printf(" [File marked broken]\n");\
        \
        return -1;\
    }
#endif

char test_prefix[] = "Running tests:";
char cutp_message[CUT_MESSAGE_SIZE];
char cutp_bar[CUT_BAR_SIZE];

typedef int (*cutp_func)();
#define UNIT_TEST(function) int function()

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
        if (status == CUT_FAIL) {
            break;
        }
#endif

        total_succeeded += status == CUT_SUCCESS;
    }

    cutp_finish_tests(total_succeeded, total_tests);
    return total_succeeded;
}

void cutp_prepare_tests(int total_test) {
    cutp_make_bar(total_test);
    cutp_print_bar();
}

void cutp_finish_tests(int total_succeeded, int total_tests) {
    cutp_clear_bar(total_tests);
    printf("\r[%s] => [%d/%d]", cutp_bar, total_succeeded, total_tests);
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
    cutp_print_bar();
}

void cutp_end_test(int index, int status) {
    char print_symbol;
    
    if (status == CUT_FAIL || status == CUT_BROKEN) {
        cutp_report_error();
    }

    print_symbol = cutp_status_to_symbol(status);
    cutp_write_to_bar(index, print_symbol);
    cutp_print_bar();
}

char cutp_status_to_symbol(int status) {
    switch (status) {
        case CUT_SUCCESS:
            return CUT_SUCCESS_SYMBOL;
        case CUT_FAIL:
            return CUT_FAIL_SYMBOL;
        case CUT_BROKEN:
            return CUT_BROKEN_SYMBOL;
    }
    return 'e';
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

void cutp_print_bar() {
    printf("\r%s [%s]", test_prefix, cutp_bar);
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
        format, "[%*s, line %*d] %s",
        CUT_FUNCTION_NAME_SIZE, info.function_name,
        CUT_LINE_NUMBER_SIZE, info.line_number,
        old_format
    );
}

#endif

