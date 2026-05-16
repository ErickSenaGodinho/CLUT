# CLUT - C Lightweight Unit Testing Library

CLUT is a simple, lightweight unit testing library for C, inspired by JUnit and Unity. It provides easy-to-use assertions and clean output.

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
    TEST_ASSERT_EQUALS_INT(5, result);
}

int main() {
    TEST_BEGIN();
    TEST_RUN(TestAddition);
    return TEST_END();
}
```

---

## Features

-[ ] Customization
    -[x] Color output
    -[ ] Custom test output streams

-[x] Boolean Assertions
    -[x] TEST_ASSERT(condition)
    -[x] TEST_ASSERT_TRUE(condition)
    -[x] TEST_ASSERT_FALSE(condition)
    -[x] TEST_ASSERT_UNLESS(condition)

-[ ] Pointer Assertions
    -[x] TEST_ASSERT_NULL(pointer)
    -[x] TEST_ASSERT_NOT_NULL(pointer)
    -[ ] Pointer comparisons

-[ ] Integer Assertions
    -[x] TEST_ASSERT_EQUALS_INT(expected, actual)
    -[ ] TEST_ASSERT_NOT_EQUALS_INT(expected, actual)
    -[x] TEST_ASSERT_GREATER_THAN_INT(expected, actual)
    -[x] TEST_ASSERT_LESS_THAN_INT(expected, actual)
    -[x] TEST_ASSERT_GREATER_OR_EQUAL_INT(expected, actual)
    -[x] TEST_ASSERT_LESS_OR_EQUAL_INT(expected, actual)

-[ ] Floating Point Assertions
    -[x] TEST_ASSERT_EQUALS_FLOAT(expected, actual)
    -[x] TEST_ASSERT_EQUALS_DOUBLE(expected, actual)
    -[x] TEST_ASSERT_GREATER_THAN_FLOAT(expected, actual)
    -[x] TEST_ASSERT_LESS_THAN_FLOAT(expected, actual)
    -[x] TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(expected, actual)
    -[x] TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(expected, actual)
    -[ ] Number within tolerance

-[x] String Assertions
    -[x] TEST_ASSERT_EQUALS_STRING(expected, actual)
    -[x] TEST_ASSERT_EQUALS_STRING_LEN(expected, actual, len)

-[ ] Memory & Array Assertions
    -[ ] Memory comparisons
    -[ ] Array comparisons

---

## Example Output

### Passing Test
Code:
```c
TEST_ASSERT_EQUALS_INT(5, 5);
```

Output:
<pre>test_file.c:10:TestAddition:<span style="color:green">PASS</span></pre>

### Failing Test
Code:
```c
TEST_ASSERT_EQUALS_INT(5, 4);
```

Output:
<pre>test_file.c:12:TestAddition:<span style="color:red">FAIL</span>:Expected 5 Received 4</pre>

### String Failure
Code:
```c
TEST_ASSERT_EQUALS_STRING("Hello", "Hella");
```

Output:
<pre>test_file.c:20:TestStrings:<span style="color:red">FAIL</span>:Expected Hello Received Hella</pre>

---

## Why CLUT?

- Minimal setup, fast results.
- Easy-to-read output.
- Supports multiple data types and comparisons.
- Inspired by JUnit and Unity, but lightweight for small projects.
