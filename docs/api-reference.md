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
Don't like the output? [reporters.md](reporters.md).

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

Heads up: CLUT keeps its state in one global, shared by every file of your program (unless
you use `CLUT_STATIC`: then each file that has the implementation gets its own copy). One
runner at a time, one thread. Don't run tests from several threads at once.

## Runner & suites

```c
RUNNER_BEGIN()                // init the runner. Once, before any suite
SUITE_BEGIN()                 // open a suite. Resets the hooks
RUN_TEST(name)                // queue a test. It runs when the suite ends
SUITE_END()                   // run the queued tests, close the suite
RUNNER_END()                  // print the summary, return number of failed tests (max 255)
```

`RUNNER_END()` is meant to be the return value of `main()`.

One runner, as many suites as you want. The runner summary adds them all up.
A suite with no tests is not run: no hooks, no output.

## Defining tests

```c
TEST(name) { ... }                                         // a single test
REPEATED_TEST(name, iterations) { ... }                    // run N times
REPEATED_TEST_WITH_THRESHOLD(name, iterations, threshold)  // run N times, tolerate failures below `threshold`
PARAM_TEST(name, Type, { v1, v2, ... }) { ... }            // run once per value

TEST_FAIL(message)                                         // fail the current test
TEST_SKIP()                                                // skip the current test
```

Inside the body:

```c
input.current_iteration      // REPEATED_TEST*: current run, 0..N-1
input.total_iterations       // REPEATED_TEST*: N
input                        // PARAM_TEST: the current value (type `Type`)
```

Iterations start at 0 everywhere, in `REPEATED_TEST*` and in `PARAM_TEST`. The same number
shows up in the failure log: `EvenNumbers[1]`.

Every test you define still needs a `RUN_TEST(name)` inside a suite.

### How a test ends

The first failed assertion aborts the test right there. Same for `TEST_SKIP()`. Nothing
after it runs. (With `CLUT_DISABLE_LONGJMP` this changes a bit, see [Configuration](#configuration).)

**`REPEATED_TEST` and `PARAM_TEST`** count per iteration:

- any iteration failed: the test failed
- every iteration skipped: the test is skipped
- otherwise it passes, and the reporter tells you how many iterations were skipped

**`REPEATED_TEST_WITH_THRESHOLD`** is for tests that are flaky and you know it. It stops as
soon as the number of failed iterations reaches `threshold`, and the test fails. Fail fewer
times than that and the test **passes**:

```c
// 100 runs. Fails on the 3rd failed iteration. 0, 1 or 2 failures: green.
REPEATED_TEST_WITH_THRESHOLD(FlakyNetwork, 100, 3) {
  TEST_ASSERT_TRUE(ping_server());
}
```

Yes, that hides real failures. Use it knowing that. A threshold of `0` means any failure
fails the test.

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

Hooks are per suite. `SUITE_BEGIN()` clears them, so the next suite starts with none.

Hooks can use assertions like a regular test. What happens when one fails:

| Hook | Fails |
|---|---|
| `before_all` | Every test in the suite is skipped. The hook itself is reported as a failed test named `before_all` |
| `before_each` | That test is not run, and it's reported as failed |
| `after_each` | That test is reported as failed |
| `after_all` | Reported as a failed test named `after_all` |

`TEST_SKIP()` only makes sense in `before_each`: that test is skipped. In the other hooks it
does nothing useful.

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

**Floats and doubles are not compared bit for bit.** `EQUAL` passes when the difference is
smaller than `expected * epsilon`. Relative, not absolute. See `CLUT_FLOAT_EPSILON` and
`CLUT_DOUBLE_EPSILON` below.

Careful with zero: if `expected` is `0`, then `actual` has to be exactly `0`. Anything else
fails, no matter how tiny. Testing something that should be "about zero"? Use
`TEST_ASSERT_WITHIN_FLOAT(0.0f, 1e-6f, actual)`.

`delta` in `WITHIN_CHAR`, `WITHIN_INT` and `WITHIN_UINT` is a `size_t`. `WITHIN_FLOAT` and
`WITHIN_DOUBLE` take a float and a double.

### Strings

```c
TEST_ASSERT_EQUAL_STRING(expected, actual)
TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len)
TEST_ASSERT_NOT_EQUAL_STRING(expected, actual)
TEST_ASSERT_NOT_EQUAL_STRING_LEN(expected, actual, len)
```

`NULL` is fine on either side. Two `NULL`s are equal. A `NULL` and a real string are not.

### Memory

```c
TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)
TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements)
```

Careful with structs: padding and uninitialized bytes will bite you.

On failure you get the byte offset of the first difference. For the `_ARRAY` version: the
element and the byte inside it.

If only one of the pointers is `NULL`, it fails. Both `NULL`, or the same pointer, passes.
A length of `0` passes too.

### Arrays

```c
TEST_ASSERT_EQUAL_<T>_ARRAY(expected, actual, num_elements)           // <T> = CHAR, INT, UINT, FLOAT, DOUBLE
TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements)
TEST_ASSERT_WITHIN_<T>_ARRAY(expected, delta, actual, num_elements)   // <T> = CHAR, INT, UINT, FLOAT, DOUBLE
```

On failure you get the index of the first element that differs.

Same rule as memory: one `NULL` pointer fails, both `NULL` or the same pointer passes, and
`num_elements` of `0` passes.

### Custom failure message

Every assertion above has a `_MESSAGE` twin with one extra trailing argument:

```c
TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, "what went wrong")
TEST_ASSERT_TRUE_MESSAGE(condition, "what went wrong")
```

What it does depends on the assertion:

- **Booleans and pointers** (`TEST_ASSERT_*`, `_TRUE`, `_FALSE`, `_UNLESS`, `_NULL`,
  `_NOT_NULL`): your text **replaces** the default message.
- **Everything else** (numbers, strings, memory, arrays, `PTR`): your text goes **in front**
  of the default message, so you still see what was expected and what you got:

```
what went wrong: Expected 5 Received 4
```

The `": "` in the middle is `CLUT_MSG_USER_SEPARATOR`.

## Reporters

What CLUT prints is decided by a reporter. The built-in ones are picked with a define (see
[Configuration](#configuration)). Want your own? Callbacks, types and examples are in
[reporters.md](reporters.md).

```c
REPORTER(name, .on_test_pass = ..., .on_test_fail = ...)   // define a reporter
USE_REPORTER(name)                                         // use it. Before RUNNER_BEGIN()
```

## Configuration

Define before including `clut.h`.

```c
// setup
CLUT_IMPLEMENTATION       // compile the implementation in this file (see [Setup](#setup))
CLUT_STATIC               // functions and internal state become `static`

// output
CLUT_OUTPUT_GITHUB        // GitHub Actions annotations
CLUT_OUTPUT_TAP           // TAP
CLUT_OUTPUT_COLOR         // ANSI colors in the default output
CLUT_STREAM_DEFAULT       // where normal output goes. Default: stdout
CLUT_STREAM_FAIL          // where failures go. Default: stderr
CLUT_MSG_USER_SEPARATOR   // between your _MESSAGE text and the default one. Default: ": "

// comparison
CLUT_FLOAT_EPSILON        // default: 1e-5f
CLUT_DOUBLE_EPSILON       // default: 1e-9

// aborting
CLUT_DISABLE_LONGJMP      // don't use setjmp/longjmp
CLUT_ABORT_TEST           // override how a test gets aborted after a failed/skipped assertion

// memory
CLUT_REALLOC              // default: realloc
CLUT_FREE                 // default: free
CLUT_DA_INIT_CAP          // initial capacity of the internal buffers. Default: 256
```

**`CLUT_STATIC`**: only visible in the file that includes the implementation. Use it if CLUT
ends up compiled in more than one file of the same program, to dodge duplicate-symbol
linkage errors. Only matters together with `CLUT_IMPLEMENTATION`.

**`CLUT_MSG_USER_SEPARATOR`**: goes between your `_MESSAGE` text and the default message. The
other `CLUT_MSG_*` macros in `clut.h` are the pieces CLUT builds its messages from. Leave
them alone.

**`CLUT_FLOAT_EPSILON`, `CLUT_DOUBLE_EPSILON`**: the relative tolerance used by every float and
double assertion that compares for equality (`EQUAL`, `NOT_EQUAL`, `GREATER_OR_EQUAL`,
`LESS_OR_EQUAL`, and the `_ARRAY` versions of `EQUAL`). `WITHIN` ignores them, it has its own
`delta`.

**`CLUT_DISABLE_LONGJMP`**: by default a failed or skipped assertion aborts the test with
`longjmp`. With this defined it falls back to a plain `return`, and that `return` only leaves
the assertion, not the test. The body keeps running. Every assertion after the first
failure does nothing, but your own code (side effects, `printf`, `free`) still runs. Use it
when `setjmp.h` isn't available on your platform.

**`CLUT_ABORT_TEST`**: defaults to `longjmp`, or `return` if `CLUT_DISABLE_LONGJMP` is defined.

**`CLUT_REALLOC`, `CLUT_FREE`**: CLUT keeps failure messages and the list of tests in
growable buffers. Using your own allocator? Define both, not just one.
