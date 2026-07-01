# CLUT - C Language Unit Testing Framework

<p align="center">CLUT is a single-header C unit testing framework inspired by JUnit and Unity. It makes writing and running tests simple, with clean, readable output.</p>

<p align="center">
    <img src="https://img.shields.io/github/actions/workflow/status/ErickSenaGodinho/CLUT/ci.yml">
    <img src="https://img.shields.io/github/license/ErickSenaGodinho/CLUT">
</p>

## Basic Usage
```c
#define CLUT_IMPLEMENTATION
#define CLUT_OUTPUT_COLOR
#include "clut.h"

TEST(Addition) {
  int result = 2 + 3;
  TEST_ASSERT_EQUAL_INT(5, result);
}

int main() {
  RUNNER_BEGIN();
    SUITE_BEGIN();
      RUN_TEST(Addition);
    SUITE_END();
  return RUNNER_END();
}
```
Output:
```
[ PASS ] Addition                                      0.000s
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
- Multiple output modes
- Hooks for test setup and teardown
- Support for repeated and parameterized tests
- Designed for small projects and fast iteration
- CI friendly

---

## Example Output

```c
TEST(Addition) {
  TEST_ASSERT_EQUAL_INT(5, 2 + 3);
}

TEST(Strings) {
  TEST_ASSERT_EQUAL_STRING("Hello", "Hella");
}

int main() {
  RUNNER_BEGIN();
    SUITE_BEGIN();
      RUN_TEST(Addition);
      RUN_TEST(Strings);
    SUITE_END();
  return RUNNER_END();
}
```

Output - Default:
```
[ PASS ] Addition                                      0.000s
[ FAIL ] Strings                                       0.000s
test_strings.c:7:Strings:Expected "Hella" to be equal to "Hello"
--------------------------------
Tests run:  2
Passed:     1
Failed:     1
--------------------------------
Total time: 0.000s
```

Output - GitHub Actions:
```
::notice title=PASS::Addition (0.000s)
::notice title=FAIL::Strings (0.000s)
::error file=test_strings.c,line=7,title=Strings::Expected "Hella" to be equal to "Hello"
::notice title=Suite Results::Tests=2 Passed=1 Failed=1 Time=0.000s
```
---

## Repeated Tests

`REPEATED_TEST` runs the same test multiple times. Each iteration receives `input`:

- `input.current_repetition` → current iteration (1..N)
- `input.total_repetitions` → total runs

### Without threshold

```c
REPEATED_TEST(EvenNumbers, 10) {
  TEST_ASSERT_TRUE(input.current_repetition % 2 == 0);
}
```

- Runs all iterations
- Marks test as failed if any iteration fails 

### With threshold

```c
REPEATED_TEST_WITH_THRESHOLD(EvenNumbers, 100, 3) {
  TEST_ASSERT_TRUE(input.current_repetition % 2 == 0);
}
```

- Stops early after reaching the failure threshold
- Useful for fast failure detection

## Parameterized Tests

`PARAM_TEST` runs the same test against multiple inputs. The type can be whatever the test needs.

```c
/* Primitive */
PARAM_TEST(IsPrime, int, { 2, 3, 5, 7, 11, 13, 97 }) {
  TEST_ASSERT_TRUE(is_prime(input));
}

/* Struct */
typedef struct { int value; int min; int max; int expected; } ClampCase;

PARAM_TEST(Clamp, ClampCase, {
    {  5, 0, 10,  5 },
    { -3, 0, 10,  0 },
    { 15, 0, 10, 10 },
}) {
  TEST_ASSERT_EQUAL_INT(input.expected, clamp(input.value, input.min, input.max));
}
```

> The current input is available as `input` inside the test body.

## Assertion Messages

CLUT supports custom failure messages for clearer test output using the `*_MESSAGE` variants:

```c
TEST_ASSERT_EQUAL_INT_MESSAGE(10, result, "Player score should be initialized correctly");
```

Output:
```
[ FAIL ] Score                                         0.000s
game_test.c:18:Score:Player score should be initialized correctly
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

## API Reference

- Test Execution Hooks
    - Hook Definition
        - BEFORE_ALL_HOOK(name)
        - BEFORE_EACH_HOOK(name)
        - AFTER_ALL_HOOK(name)
        - AFTER_EACH_HOOK(name)

    - Hook Registration
        - SET_BEFORE_ALL(hook)
        - SET_BEFORE_EACH(hook)
        - SET_AFTER_ALL(hook)
        - SET_AFTER_EACH(hook)

- Boolean Assertions
    - TEST_ASSERT(condition)
    - TEST_ASSERT_TRUE(condition)
    - TEST_ASSERT_FALSE(condition)
    - TEST_ASSERT_UNLESS(condition)
    - TEST_FAIL(message)

- Pointer Assertions
    - TEST_ASSERT_NULL(pointer)
    - TEST_ASSERT_NOT_NULL(pointer)
    - TEST_ASSERT_EQUAL_PTR(expected, actual)
    - TEST_ASSERT_NOT_EQUAL_PTR(expected, actual)

- Char Assertions
    - TEST_ASSERT_EQUAL_CHAR(expected, actual)
    - TEST_ASSERT_NOT_EQUAL_CHAR(expected, actual)
    - TEST_ASSERT_GREATER_THAN_CHAR(threshold, actual)
    - TEST_ASSERT_LESS_THAN_CHAR(threshold, actual)
    - TEST_ASSERT_GREATER_OR_EQUAL_CHAR(threshold, actual)
    - TEST_ASSERT_LESS_OR_EQUAL_CHAR(threshold, actual)
    - TEST_ASSERT_WITHIN_CHAR(expected, delta, actual)

- Integer Assertions
    - TEST_ASSERT_EQUAL_INT(expected, actual)
    - TEST_ASSERT_NOT_EQUAL_INT(expected, actual)
    - TEST_ASSERT_GREATER_THAN_INT(threshold, actual)
    - TEST_ASSERT_LESS_THAN_INT(threshold, actual)
    - TEST_ASSERT_GREATER_OR_EQUAL_INT(threshold, actual)
    - TEST_ASSERT_LESS_OR_EQUAL_INT(threshold, actual)
    - TEST_ASSERT_WITHIN_INT(expected, delta, actual)

- Unsigned Integer Assertions
    - TEST_ASSERT_EQUAL_UINT(expected, actual)
    - TEST_ASSERT_NOT_EQUAL_UINT(expected, actual)
    - TEST_ASSERT_GREATER_THAN_UINT(threshold, actual)
    - TEST_ASSERT_LESS_THAN_UINT(threshold, actual)
    - TEST_ASSERT_GREATER_OR_EQUAL_UINT(threshold, actual)
    - TEST_ASSERT_LESS_OR_EQUAL_UINT(threshold, actual)
    - TEST_ASSERT_WITHIN_UINT(expected, delta, actual)

- Floating Point Assertions
    - TEST_ASSERT_EQUAL_FLOAT(expected, actual)
    - TEST_ASSERT_NOT_EQUAL_FLOAT(expected, actual)
    - TEST_ASSERT_GREATER_THAN_FLOAT(threshold, actual)
    - TEST_ASSERT_LESS_THAN_FLOAT(threshold, actual)
    - TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(threshold, actual)
    - TEST_ASSERT_LESS_OR_EQUAL_FLOAT(threshold, actual)
    - TEST_ASSERT_WITHIN_FLOAT(expected, delta, actual)
    - TEST_ASSERT_EQUAL_DOUBLE(expected, actual)
    - TEST_ASSERT_NOT_EQUAL_DOUBLE(expected, actual)
    - TEST_ASSERT_GREATER_THAN_DOUBLE(threshold, actual)
    - TEST_ASSERT_LESS_THAN_DOUBLE(threshold, actual)
    - TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(threshold, actual)
    - TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(threshold, actual)
    - TEST_ASSERT_WITHIN_DOUBLE(expected, delta, actual)

- String Assertions
    - TEST_ASSERT_EQUAL_STRING(expected, actual)
    - TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len)
    - TEST_ASSERT_NOT_EQUAL_STRING(expected, actual)
    - TEST_ASSERT_NOT_EQUAL_STRING_LEN(expected, actual, len)

- Memory Assertions (Comparing structs directly may be affected by padding and uninitialized bytes)
    - TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)
    - TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements)

- Array Assertions
    - TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, actual, num_elements)
    - TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements)
    - TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements)
    - TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements)
    - TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, num_elements)
    - TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements)
    - TEST_ASSERT_WITHIN_CHAR_ARRAY(expected, delta, actual, num_elements)
    - TEST_ASSERT_WITHIN_INT_ARRAY(expected, delta, actual, num_elements)
    - TEST_ASSERT_WITHIN_UINT_ARRAY(expected, delta, actual, num_elements)
    - TEST_ASSERT_WITHIN_FLOAT_ARRAY(expected, delta, actual, num_elements)
    - TEST_ASSERT_WITHIN_DOUBLE_ARRAY(expected, delta, actual, num_elements)
