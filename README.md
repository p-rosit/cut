# C Unit Testing (cut)

A simple framework for unit testing in C. To use this library include the header file in your project. A unit test function can be declared in the following way

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
    [other names...]
)
```

