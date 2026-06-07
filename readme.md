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
[ PASS ] TestAddition                                      0.000s
--------------------------------
Tests run:  1
Passed:     1
Failed:     0
--------------------------------
Total time: 0.000s
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

```c
void TestAddition() {
    TEST_ASSERT_EQUAL_INT(5, 2 + 3);
}

void TestStrings() {
    TEST_ASSERT_EQUAL_STRING("Hello", "Hella");
}

int main() {
    TEST_BEGIN();
    TEST_RUN(TestAddition);
    TEST_RUN(TestStrings);
    return TEST_END();
}
```

Output:
```
[ PASS ] TestAddition                                      0.000s
[ FAIL ] TestStrings                                       0.000s
test_strings.c:7:TestStrings:Expected "Hella" to be equal to "Hello"
--------------------------------
Tests run:  2
Passed:     1
Failed:     1
--------------------------------
Total time: 0.000s
```
---

## Assertion Messages

CLUT supports custom failure messages for clearer test output using the `*_MESSAGE` variants:

```c
TEST_ASSERT_EQUAL_INT_MESSAGE(10, result, "Player score should be initialized correctly");
```

Output:
```
[ FAIL ] TestScore                                         0.000s
game_test.c:18:TestScore:Player score should be initialized correctly
```

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

