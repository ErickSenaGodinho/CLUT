# CLUT - C Lightweight Unit Testing Library

CLUT is a lightweight, header-only C unit testing library inspired by JUnit and Unity. It makes writing and running tests simple, with clean, readable output.

## Installation
```c
#define CLUT_IMPLEMENTATION
#define CLUT_OUTPUT_COLOR
#include "clut.h"
```

## Basic Usage
```c
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

---

## Features

- [ ] Customization
    - [x] Color output
    - [ ] Custom test output streams

- [x] Boolean Assertions
    - [x] TEST_ASSERT(condition)
    - [x] TEST_ASSERT_TRUE(condition)
    - [x] TEST_ASSERT_FALSE(condition)
    - [x] TEST_ASSERT_UNLESS(condition)

- [ ] Pointer Assertions
    - [x] TEST_ASSERT_NULL(pointer)
    - [x] TEST_ASSERT_NOT_NULL(pointer)
    - [x] TEST_ASSERT_EQUAL_PTR(expected, actual)
    - [x] TEST_ASSERT_NOT_EQUAL_PTR(expected, actual)
    - [ ] Pointer comparisons

- [ ] Integer Assertions
    - [x] TEST_ASSERT_EQUAL_INT(expected, actual)
    - [X] TEST_ASSERT_NOT_EQUAL_INT(expected, actual)
    - [x] TEST_ASSERT_GREATER_THAN_INT(expected, actual)
    - [x] TEST_ASSERT_LESS_THAN_INT(expected, actual)
    - [x] TEST_ASSERT_GREATER_OR_EQUAL_INT(expected, actual)
    - [x] TEST_ASSERT_LESS_OR_EQUAL_INT(expected, actual)

- [ ] Floating Point Assertions
    - [x] TEST_ASSERT_EQUAL_FLOAT(expected, actual)
    - [x] TEST_ASSERT_EQUAL_DOUBLE(expected, actual)
    - [x] TEST_ASSERT_GREATER_THAN_FLOAT(expected, actual)
    - [x] TEST_ASSERT_LESS_THAN_FLOAT(expected, actual)
    - [x] TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(expected, actual)
    - [x] TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(expected, actual)
    - [X] TEST_ASSERT_NOT_EQUAL_FLOAT(expected, actual)
    - [X] TEST_ASSERT_NOT_EQUAL_DOUBLE(expected, actual)
    - [ ] Number within tolerance

- [x] String Assertions
    - [x] TEST_ASSERT_EQUAL_STRING(expected, actual)
    - [x] TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len)
    - [x] TEST_ASSERT_NOT_EQUAL_STRING(expected, actual)
    - [x] TEST_ASSERT_NOT_EQUAL_STRING_LEN(expected, actual, len)

- [ ] Memory & Array Assertions
    - [ ] Memory comparisons
    - [ ] Array comparisons

---

## Example Output

### Passing Test
Code:
```c
TEST_ASSERT_EQUALS_INT(5, 5);
```

Output:
<pre>test_file.c:10:TestAddition:PASS</pre>

### Failing Test
Code:
```c
TEST_ASSERT_EQUALS_INT(5, 4);
```

Output:
<pre>test_file.c:12:TestAddition:FAIL:Expected 5 Received 4</pre>

### String Failure
Code:
```c
TEST_ASSERT_EQUALS_STRING("Hello", "Hella");
```

Output:
<pre>test_file.c:20:TestStrings:FAIL:Expected Hella to be equal to Hello</pre>

---

## Why CLUT?

- Minimal setup, fast results.
- Easy-to-read output.
- Supports multiple data types and comparisons.
- Inspired by JUnit and Unity, but lightweight for small projects.
