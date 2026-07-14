# API Reference

Every macro CLUT exposes to code that includes `clut.h`. For a narrative walkthrough
with examples, see [guide.md](guide.md).

## Runner & Suite

These wrap the `main()` of your test binary and control the overall test run.

- `RUNNER_BEGIN()` - initializes the runner. Call once, before any suite.
- `SUITE_BEGIN()` - starts a suite/group of tests.
- `RUN_TEST(name)` - registers a test (created with `TEST`, `REPEATED_TEST`,
  `REPEATED_TEST_WITH_THRESHOLD`, or `PARAM_TEST`) to run when the suite ends.
- `SUITE_END()` - runs all registered tests and closes the current suite.
- `RUNNER_END()` - prints the final summary and returns the number of failed tests
  (limited to `255`). Use as the return value of `main()`.
  
```c
int main() {
  RUNNER_BEGIN();
    SUITE_BEGIN();
      RUN_TEST(Addition);
    SUITE_END();
  return RUNNER_END();
}
```

> If you don't want to write this by hand, `runner_generator` builds this whole block
> for you - see [runner_generator.md](runner_generator.md).

## Test Execution Hooks

### Hook Definition

- `BEFORE_ALL_HOOK(name)` - runs once, before the first test in the enclosing suite.
- `BEFORE_EACH_HOOK(name)` - runs before every test in the enclosing suite.
- `AFTER_ALL_HOOK(name)` - runs once, after the last test in the enclosing suite.
- `AFTER_EACH_HOOK(name)` - runs after every test in the enclosing suite.

### Hook Registration

- `SET_BEFORE_ALL(hook)`
- `SET_BEFORE_EACH(hook)`
- `SET_AFTER_ALL(hook)`
- `SET_AFTER_EACH(hook)`

## Test Definition

- `TEST(name)` - defines a single test.
- `REPEATED_TEST(name, repetitions)` - runs the same test `repetitions` times.
- `REPEATED_TEST_WITH_THRESHOLD(name, repetitions, threshold)` - same as above, but stops
  early once `threshold` failures have been reached.
- `PARAM_TEST(name, type, { ...values })` - runs the same test once per value in the list.

See [guide.md](guide.md#repeated-tests) for full examples of repeated and parameterized tests.

## Boolean Assertions

- `TEST_ASSERT(condition)`
- `TEST_ASSERT_TRUE(condition)`
- `TEST_ASSERT_FALSE(condition)`
- `TEST_ASSERT_UNLESS(condition)`
- `TEST_FAIL(message)`

## Pointer Assertions

- `TEST_ASSERT_NULL(pointer)`
- `TEST_ASSERT_NOT_NULL(pointer)`
- `TEST_ASSERT_EQUAL_PTR(expected, actual)`
- `TEST_ASSERT_NOT_EQUAL_PTR(expected, actual)`

## Char Assertions

- `TEST_ASSERT_EQUAL_CHAR(expected, actual)`
- `TEST_ASSERT_NOT_EQUAL_CHAR(expected, actual)`
- `TEST_ASSERT_GREATER_THAN_CHAR(threshold, actual)`
- `TEST_ASSERT_LESS_THAN_CHAR(threshold, actual)`
- `TEST_ASSERT_GREATER_OR_EQUAL_CHAR(threshold, actual)`
- `TEST_ASSERT_LESS_OR_EQUAL_CHAR(threshold, actual)`
- `TEST_ASSERT_WITHIN_CHAR(expected, delta, actual)`

## Integer Assertions

- `TEST_ASSERT_EQUAL_INT(expected, actual)`
- `TEST_ASSERT_NOT_EQUAL_INT(expected, actual)`
- `TEST_ASSERT_GREATER_THAN_INT(threshold, actual)`
- `TEST_ASSERT_LESS_THAN_INT(threshold, actual)`
- `TEST_ASSERT_GREATER_OR_EQUAL_INT(threshold, actual)`
- `TEST_ASSERT_LESS_OR_EQUAL_INT(threshold, actual)`
- `TEST_ASSERT_WITHIN_INT(expected, delta, actual)`

## Unsigned Integer Assertions

- `TEST_ASSERT_EQUAL_UINT(expected, actual)`
- `TEST_ASSERT_NOT_EQUAL_UINT(expected, actual)`
- `TEST_ASSERT_GREATER_THAN_UINT(threshold, actual)`
- `TEST_ASSERT_LESS_THAN_UINT(threshold, actual)`
- `TEST_ASSERT_GREATER_OR_EQUAL_UINT(threshold, actual)`
- `TEST_ASSERT_LESS_OR_EQUAL_UINT(threshold, actual)`
- `TEST_ASSERT_WITHIN_UINT(expected, delta, actual)`

## Floating Point Assertions

- `TEST_ASSERT_EQUAL_FLOAT(expected, actual)`
- `TEST_ASSERT_NOT_EQUAL_FLOAT(expected, actual)`
- `TEST_ASSERT_GREATER_THAN_FLOAT(threshold, actual)`
- `TEST_ASSERT_LESS_THAN_FLOAT(threshold, actual)`
- `TEST_ASSERT_GREATER_OR_EQUAL_FLOAT(threshold, actual)`
- `TEST_ASSERT_LESS_OR_EQUAL_FLOAT(threshold, actual)`
- `TEST_ASSERT_WITHIN_FLOAT(expected, delta, actual)`
- `TEST_ASSERT_EQUAL_DOUBLE(expected, actual)`
- `TEST_ASSERT_NOT_EQUAL_DOUBLE(expected, actual)`
- `TEST_ASSERT_GREATER_THAN_DOUBLE(threshold, actual)`
- `TEST_ASSERT_LESS_THAN_DOUBLE(threshold, actual)`
- `TEST_ASSERT_GREATER_OR_EQUAL_DOUBLE(threshold, actual)`
- `TEST_ASSERT_LESS_OR_EQUAL_DOUBLE(threshold, actual)`
- `TEST_ASSERT_WITHIN_DOUBLE(expected, delta, actual)`

## String Assertions

- `TEST_ASSERT_EQUAL_STRING(expected, actual)`
- `TEST_ASSERT_EQUAL_STRING_LEN(expected, actual, len)`
- `TEST_ASSERT_NOT_EQUAL_STRING(expected, actual)`
- `TEST_ASSERT_NOT_EQUAL_STRING_LEN(expected, actual, len)`

## Memory Assertions

> Comparing structs directly may be affected by padding and uninitialized bytes.

- `TEST_ASSERT_EQUAL_MEMORY(expected, actual, len)`
- `TEST_ASSERT_EQUAL_MEMORY_ARRAY(expected, actual, len, num_elements)`

## Array Assertions

- `TEST_ASSERT_EQUAL_CHAR_ARRAY(expected, actual, num_elements)`
- `TEST_ASSERT_EQUAL_INT_ARRAY(expected, actual, num_elements)`
- `TEST_ASSERT_EQUAL_UINT_ARRAY(expected, actual, num_elements)`
- `TEST_ASSERT_EQUAL_FLOAT_ARRAY(expected, actual, num_elements)`
- `TEST_ASSERT_EQUAL_DOUBLE_ARRAY(expected, actual, num_elements)`
- `TEST_ASSERT_EQUAL_STRING_ARRAY(expected, actual, num_elements)`
- `TEST_ASSERT_WITHIN_CHAR_ARRAY(expected, delta, actual, num_elements)`
- `TEST_ASSERT_WITHIN_INT_ARRAY(expected, delta, actual, num_elements)`
- `TEST_ASSERT_WITHIN_UINT_ARRAY(expected, delta, actual, num_elements)`
- `TEST_ASSERT_WITHIN_FLOAT_ARRAY(expected, delta, actual, num_elements)`
- `TEST_ASSERT_WITHIN_DOUBLE_ARRAY(expected, delta, actual, num_elements)`

## Assertion Messages

Every assertion above has a `*_MESSAGE` variant taking an extra trailing string
argument, printed instead of the default message on failure:

```c
TEST_ASSERT_EQUAL_INT_MESSAGE(expected, actual, message)
```

See [guide.md](guide.md#assertion-messages) for a worked example.

## CLI Tool

`runner_generator` is not a macro from `clut.h` - it's a separate command-line tool
shipped alongside CLUT. See [runner_generator.md](runner_generator.md).
