# C Unit Testing (cut)

A simple framework for unit testing in C. To write unit tests you only need to include `cut.h` wherever you want it. Optionally a bash script is also supplied, `run_cut.sh`, which runs all tests under a directory. Some flags can also be used with `run_cut.sh` such as continue on error, compiler arguments and so on.

By default the error message cannot contain more than 2047 characters and a test file cannot contain more than 512 tests. This should not be too much of a restriction since error messages are not informative at that size and one will barely be able to have any kind of overview of the tests if there are 512 of them in a single file :)

## Writing a test

A very simple test can be written in the following way:

```c
UNIT_TEST(function_name) {
    int a, b;

    a = 2;
    b = 1 + 1;

    ASSERT_EQUAL(a, b, "Expected equal values, got a = %d, b = %d.", a, b);

    UNIT_TEST_END;
}
```

in the bottom of the file you must then specify which functions to run in the following way

```c
LIST_TESTS(
    function_name,
    [other functions...]
)
```

If an assert fails we assume that the rest of the test function is no longer valid so the test function returns. If a single test fails no other tests in that file are run unless the macro `CUT_CONTINUE_ON_FAIL` has been defined.

One can also define sub tests which are tests that can be called from another test, any amount of sub tests can be nested but if a single one fails the call stack is printed as the function name. Sub tests can do anything a normal test can, expect be listed in `LIST_TESTS(...)` since it assumes those functions have been declared with `UNIT_TEST(name)`.

## Example tests

Examples of tests, which double as tests for this library, can be found here [here](https://github.com/p-rosit/cut/blob/main/tests/test_succeeding_examples.c) for tests that should succeed and [here](https://github.com/p-rosit/cut/blob/main/tests/test_failing_examples.c) for tests that should fail.

Additionally one can define sub tests which can take input, an example of this can be found [here](https://github.com/p-rosit/cut/blob/main/tests/test_sub_tests.c).

To run these tests one can navigate to `/cut` and run `./run_cut.sh` to see the testing output.

## Running tests

By default testing is aborted if a single test fails, to continue on fail define the flag `CUT_CONTINUE_ON_FAIL` either when compiling or before importing `cut.h` or by using the `-e` flag with `run_cut.sh`.

To compile and run all tests in a directory one can use the bash script `run_cut.sh`. All directories that match `test*` are assumed to only contain files to be tested, these will be compiled and run by the script. All files that match `test*.c` are assumed to be test files which will be compiled and run. To see specific usage of the script run `run_cut.sh -h`.

Tests can also be run on your own by compiling the test file and running the compiled code.

## Available macros

The available assert macros are the following

| Assert                                    |
| ----------------------------------------- |
| ASSERT_NULL(ptr, fmt, ...)                |
| ASSERT_NOT_NULL(ptr, fmt, ...)            |
| ASSERT_TRUE(val, fmt, ...)                |
| ASSERT_FALSE(val, fmt, ...)               |
| ASSERT_EQUAL(val1, val2, fmt, ...)        |
| ASSERT_NOT_EQUAL(val1, val2, fmt, ...)    |

every assert expects one (or two for equality) values as the first inputs(s). A formatting string to be printed if the test fails is the input after that and then a variable amount of arguments (even zero) can be given to the macro as input just like printf.

The available macros for tests are:

| Macro                     | Usage                                                                         |
| ------------------------- | ----------------------------------------------------------------------------- |
| LIST_TESTS(...)           | Contains all functions to test.                                               |
| UNIT_TEST(func_name)      | Declaration of a unit test.                                                   |
| SUB_TEST(func_name, ...)  | Declaration of sub tests, variable arguments are arguments to the function.   |
| CALL_TEST(func_name, ...) | Sub test call, variable arguments are arguments to the function call.         |
| TEST_END                  | Marks the successful end of a test.                                           |
| TEST_BROKEN               | Marks a test as broken.                                                       |
| TEST_FAIL(fmt, ...)       | Fails a test and prints an error message.                                     |
| CUT_FILE_BROKEN           | Marks an entire file as broken if defined before including `cut.h`            |

All unit tests must end with `TEST_END;` and if a unit tests is broken (as sometimes happens) one can write `TEST_BROKEN;` as (one of) the first line(s) of the test. If an entire file should be marked as broken one can define the symbol `CUT_FILE_BROKEN` before including `cut.h` which will skip all the tests.

## Reserved names

Other than the previous macros functions and structs internal to the library start with `cutp_` and macros which are internal to the library start with `CUT_`.

