# C Unit Testing (cut)

A simple framework for unit testing in C. To use this library include the header file in your project.

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
UNIT_TESTS(
    function_name,
    [other functions...]
)
```

## Example tests

Examples of tests, which double as tests for this library, can be found here [here](https://github.com/p-rosit/cut/blob/main/tests/test_succeeding_examples.c) for tests that should succeed and [here](https://github.com/p-rosit/cut/blob/main/tests/test_failing_examples.c) for tests that should fail.

## Running tests

By default testing is aborted if a single test fails, to continue on fail define the flag `UNITTEST_CONTINUE_ON_FAIL` either when compiling or before importing `cut.h`.

To compile and run all tests in a directory one can use the bash script `run_cut.sh`. All directories starting with `tests` are assumed to only contain files to be tested, these will be compiled and run by the script. All files that match `test*.c` are assumed to be test files which will be compiled and run. To see specific usage of the script run `./run_cut.sh -h`.

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

All unit tests must end with `UNIT_TEST_END;` and if a unit tests is broken (as sometimes happens) one can write `UNIT_TEST_BROKEN;` as the first line of the test. If an entire file should be marked as broken one can define the symbol `UNIT_TEST_FILE_BROKEN` which will skip all the tests.
