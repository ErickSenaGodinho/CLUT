# CLUT - C Language Unit Testing Framework

<p align="center">CLUT is a header-only C unit testing framework inspired by JUnit and Unity. It makes writing and running tests simple, with clean, readable output.</p>

<p align="center">
    <img src="https://img.shields.io/github/actions/workflow/status/ErickSenaGodinho/CLUT/ci.yml">
    <img src="https://img.shields.io/github/license/ErickSenaGodinho/CLUT">
</p>

## Basic Usage
```c
#define CLUT_IMPLEMENTATION
#define CLUT_OUTPUT_COLOR
#include "clut.h"

void TestAddition() {
    int result = 2 + 3;
    TEST_ASSERT_EQUAL_INT(5, result);
}

int main() {
    TEST_BEGIN();
    TEST_RUN(TestAddition);
    return TEST_END();
}
```
Output:
```
basic_usage.c:12:TestAddition:PASS
------------------------
1 Tests 0 Failures - Total Time 0.00s
```
---

## Why CLUT?

Most C testing frameworks either require extra setup, multiple files or additional tooling.

CLUT keeps things simple:

- Single header integration
- No external dependencies
- Readable test output
- Hooks for test setup and teardown
- Designed for small projects and fast iteration
- CI friendly

---

## Example Output

### Passing Test
Code:
```c
TEST_ASSERT_EQUAL_INT(5, 5);
```

Output:
<pre>test_file.c:10:TestAddition:PASS</pre>

### Failing Test
Code:
```c
TEST_ASSERT_EQUAL_INT(5, 4);
```

Output:
<pre>test_file.c:12:TestAddition:FAIL:Expected 5 Received 4</pre>

### String Failure
Code:
```c
TEST_ASSERT_EQUAL_STRING("Hello", "Hella");
```

Output:
<pre>test_file.c:20:TestStrings:FAIL:Expected "Hella" to be equal to "Hello"</pre>

---

## Assertion Messages

CLUT also supports custom failure messages for clearer test output `*_MESSAGE`

```c
TEST_ASSERT_EQUAL_INT_MESSAGE(10, result, "Player score should be initialized correctly");
```

Output:
<pre>
game_test.c:18:TestScore:FAIL:Player score should be initialized correctly
</pre>

## Project Structure

```text
CLUT/
├── .github/
│   └── workflows/
│       └── ci.yml
├── examples/
│   └── basic_usage.c
├── tests/
│   └── test_clut.c
├── clut.h
├── readme.md
└── LICENSE
```

---

## Assertion Reference

- [x] Test Execution Hooks
    - [x] CLUT_BEFORE_ALL(hook_fn)
    - [x] CLUT_BEFORE_EACH(hook_fn)
    - [x] CLUT_AFTER_ALL(hook_fn)
    - [x] CLUT_AFTER_EACH(hook_fn)

- [x] Boolean Assertions
    - [x] TEST_ASSERT(condition)
    - [x] TEST_ASSERT_TRUE(condition)
    - [x] TEST_ASSERT_FALSE(condition)
    - [x] TEST_ASSERT_UNLESS(condition)

- [x] Pointer Assertions
    - [x] TEST_ASSERT_NULL(pointer)
    - [x] TEST_ASSERT_NOT_NULL(pointer)
    - [x] TEST_ASSERT_EQUAL_PTR(expected, actual)
    - [x] TEST_ASSERT_NOT_EQUAL_PTR(expected, actual)

- [x] Char Assertions
    - [x] TEST_ASSERT_EQUAL_CHAR(expected, actual)
    - [x] TEST_ASSERT_NOT_EQUAL_CHAR(expected, actual)
    - [x] TEST_ASSERT_WITHIN_CHAR(expected, delta, actual)

- [x] Integer Assertions
    - [x] TEST_ASSERT_EQUAL_INT(expected, actual)
    - [x] TEST_ASSERT_NOT_EQUAL_INT(expected, actual)
    - [x] TEST_ASSERT_GREATER_THAN_INT(expected, actual)
    - [x] TEST_ASSERT_LESS_THAN_INT(expected, actual)
    - [x] TEST_ASSERT_GREATER_OR_EQUAL_INT(expected, actual)
    - [x] TEST_ASSERT_LESS_OR_EQUAL_INT(expected, actual)
    - [x] TEST_ASSERT_WITHIN_INT(expected, delta, actual)

- [x] Unsigned Integer Assertions
    - [x] TEST_ASSERT_EQUAL_UINT(expected, actual)
    - [x] TEST_ASSERT_NOT_EQUAL_UINT(expected, actual)
    - [x] TEST_ASSERT_GREATER_THAN_UINT(expected, actual)
    - [x] TEST_ASSERT_LESS_THAN_UINT(expected, actual)
    - [x] TEST_ASSERT_GREATER_OR_EQUAL_UINT(expected, actual)
    - [x] TEST_ASSERT_LESS_OR_EQUAL_UINT(expected, actual)
    - [x] TEST_ASSERT_WITHIN_UINT(expected, delta, actual)

- [x] Floating Point Assertions
    - [x] TEST_ASSERT_EQUAL_FLOAT(expected, actual)
    - [x] TEST_ASSERT_NOT_EQUAL_FLOAT(expected, actual)
    - [x] TEST_ASSERT_GREATER_THAN_FLOAT(expected, actual)
    - [x] TEST_ASSERT_LESS_THAN_FLOAT(expected, actual)
    - [x] TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(expected, actual)
    - [x] TEST_ASSERT_LESS_OR_EQUAL_FLOAT(expected, actual)
    - [x] TEST_ASSERT_WITHIN_FLOAT(expected, delta, actual)
    - [x] TEST_ASSERT_EQUAL_DOUBLE(expected, actual)
    - [x] TEST_ASSERT_NOT_EQUAL_DOUBLE(expected, actual)
    - [x] TEST_ASSERT_GREATER_THAN_DOUBLE(expected, actual)
    - [x] TEST_ASSERT_LESS_THAN_DOUBLE(expected, actual)
    - [x] TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(expected, actual)
    - [x] TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(expected, actual)
    - [x] TEST_ASSERT_WITHIN_DOUBLE(expected, delta, actual)

- [x] String Assertions
    - [x] TEST_ASSERT_EQUAL_STRING(expected, actual)
    - [x] TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len)
    - [x] TEST_ASSERT_NOT_EQUAL_STRING(expected, actual)
    - [x] TEST_ASSERT_NOT_EQUAL_STRING_LEN(expected, actual, len)

- [x] Memory Assertions (Comparing structs directly may be affected by padding and uninitialized bytes)
    - [x] TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)
    - [x] TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements)

- [x] Array Assertions
    - [x] TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, actual, num_elements)
    - [x] TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements)
    - [x] TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements)
    - [x] TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements)
    - [x] TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, num_elements)
    - [x] TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements)
    - [x] TEST_ASSERT_WITHIN_CHAR_ARRAY(expected, delta, actual, num_elements)
    - [x] TEST_ASSERT_WITHIN_INT_ARRAY(expected, delta, actual, num_elements)
    - [x] TEST_ASSERT_WITHIN_UINT_ARRAY(expected, delta, actual, num_elements)
    - [x] TEST_ASSERT_WITHIN_FLOAT_ARRAY(expected, delta, actual, num_elements)
    - [x] TEST_ASSERT_WITHIN_DOUBLE_ARRAY(expected, delta, actual, num_elements)

