# CLUT API cheatsheet

Single-header unit testing for C. Everything below is a macro from `clut.h`.
Want examples instead of a list? Read [guide.md](guide.md).

```c
#define CLUT_IMPLEMENTATION
#include "clut.h"

TEST(Addition) {
  TEST_ASSERT_EQUAL_INT(5, 2 + 3);
}

int main() {
  RUNNER_BEGIN();
    SUITE_BEGIN();
      RUN_TEST(Addition);
    SUITE_END();
  return RUNNER_END();
}
```

Don't want to write that `main()` by hand? [runner_generator.md](runner_generator.md).

---

## Setup

CLUT is header-only, stb style. `clut.h` alone gives you declarations. To get the
implementation, define `CLUT_IMPLEMENTATION` before the include, in **one** file only:

```c
#define CLUT_IMPLEMENTATION
#include "clut.h"
```

Other files just `#include "clut.h"`. If you use `runner_generator`, the generated runner
already does this for you.

## Runner & suites

```c
RUNNER_BEGIN()                // init the runner. Once, before any suite
SUITE_BEGIN()                 // open a suite
RUN_TEST(name)                // queue a test. It runs when the suite ends
SUITE_END()                   // run the queued tests, close the suite
RUNNER_END()                  // print the summary, return number of failed tests (max 255)
```

`RUNNER_END()` is meant to be the return value of `main()`.

## Defining tests

```c
TEST(name) { ... }                                         // a single test
REPEATED_TEST(name, repetitions) { ... }                   // run N times
REPEATED_TEST_WITH_THRESHOLD(name, repetitions, threshold) // same, but stop at `threshold` failures
PARAM_TEST(name, Type, { v1, v2, ... }) { ... }            // run once per value

TEST_FAIL(message)                                         // fail the current test
TEST_SKIP()                                                // skip the current test
```

Inside the body:

```c
input.current_repetition     // REPEATED_TEST*: current run, 1..N
input.total_repetitions      // REPEATED_TEST*: N
input                        // PARAM_TEST: the current value (type `Type`)
```

Every test you define still needs a `RUN_TEST(name)` inside a suite.

## Hooks

Define a hook, then plug it into a suite with the matching `USE_HOOK_*`.
The names have to match: `USE_HOOK_BEFORE_EACH(x)` only takes a `HOOK_BEFORE_EACH(x)`.

```c
// define
HOOK_BEFORE_ALL(name)         // once, before the first test of the suite
HOOK_BEFORE_EACH(name)        // before every test of the suite
HOOK_AFTER_ALL(name)          // once, after the last test of the suite
HOOK_AFTER_EACH(name)         // after every test of the suite

// use (inside SUITE_BEGIN / SUITE_END)
USE_HOOK_BEFORE_ALL(name)
USE_HOOK_BEFORE_EACH(name)
USE_HOOK_AFTER_ALL(name)
USE_HOOK_AFTER_EACH(name)
```

```c
HOOK_BEFORE_EACH(reset_state) {
  counter = 0;
}

int main() {
  RUNNER_BEGIN();
    SUITE_BEGIN();
      USE_HOOK_BEFORE_EACH(reset_state);
      RUN_TEST(Addition);
    SUITE_END();
  return RUNNER_END();
}
```

## Assertions

`<T>` is one of: `CHAR`, `INT`, `UINT`, `FLOAT`, `DOUBLE`.

### Booleans and pointers

```c
TEST_ASSERT(condition)
TEST_ASSERT_TRUE(condition)
TEST_ASSERT_FALSE(condition)
TEST_ASSERT_UNLESS(condition)             // fails if condition is true

TEST_ASSERT_NULL(pointer)
TEST_ASSERT_NOT_NULL(pointer)
TEST_ASSERT_EQUAL_PTR(expected, actual)
TEST_ASSERT_NOT_EQUAL_PTR(expected, actual)
```

### Numbers (`<T>` = CHAR, INT, UINT, FLOAT, DOUBLE)

```c
TEST_ASSERT_EQUAL_<T>(expected, actual)
TEST_ASSERT_NOT_EQUAL_<T>(expected, actual)
TEST_ASSERT_GREATER_THAN_<T>(threshold, actual)
TEST_ASSERT_LESS_THAN_<T>(threshold, actual)
TEST_ASSERT_GREATER_OR_EQUAL_<T>(threshold, actual)
TEST_ASSERT_LESS_OR_EQUAL_<T>(threshold, actual)
TEST_ASSERT_WITHIN_<T>(expected, delta, actual)
```

So `TEST_ASSERT_LESS_THAN_FLOAT(...)` and `TEST_ASSERT_WITHIN_UINT(...)` both exist.

### Strings

```c
TEST_ASSERT_EQUAL_STRING(expected, actual)
TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len)
TEST_ASSERT_NOT_EQUAL_STRING(expected, actual)
TEST_ASSERT_NOT_EQUAL_STRING_LEN(expected, actual, len)
```

### Memory

```c
TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)
TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements)
```

Careful with structs: padding and uninitialized bytes will bite you.

### Arrays

```c
TEST_ASSERT_EQUAL_<T>_ARRAY(expected, actual, num_elements)           // <T> = CHAR, INT, UINT, FLOAT, DOUBLE
TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements)
TEST_ASSERT_WITHIN_<T>_ARRAY(expected, delta, actual, num_elements)   // <T> = CHAR, INT, UINT, FLOAT, DOUBLE
```

### Custom failure message

Every assertion above has a `_MESSAGE` twin with one extra trailing argument.
It replaces the default message on failure:

```c
TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, "what went wrong")
TEST_ASSERT_TRUE_MESSAGE(condition, "what went wrong")
```

## Configuration

Define before including `clut.h`.

```c
CLUT_IMPLEMENTATION     // compile the implementation in this file (see Setup)
CLUT_STATIC             // functions and internal state become `static`
CLUT_DISABLE_LONGJMP    // don't use setjmp/longjmp
CLUT_ABORT_TEST         // override how a test gets aborted after a failed/skipped assertion
```

**`CLUT_STATIC`**: only visible in the file that includes the implementation. Use it if CLUT
ends up compiled in more than one file of the same program, to dodge duplicate-symbol
linkage errors. Only matters together with `CLUT_IMPLEMENTATION`.

**`CLUT_DISABLE_LONGJMP`**: by default a failed or skipped assertion aborts the test with
`longjmp`. With this defined it falls back to a plain `return`.

**`CLUT_ABORT_TEST`**: defaults to `longjmp`, or `return` if `CLUT_DISABLE_LONGJMP` is defined.
