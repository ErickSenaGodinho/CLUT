# CLUT guide

Examples first, explanation second. For the full list of macros see
[api-reference.md](api-reference.md).

## Your first test

```c
#define CLUT_IMPLEMENTATION
#include "clut.h"

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

```sh
gcc test.c -o test && ./test
```

```
[ PASS ] Addition                                      0.000s
[ FAIL ] Strings                                       0.000s
test_strings.c:7:Strings:Expected "Hella" to be equal to "Hello"
--------------------------------
Tests run:  2
Passed:     1
Failed:     1
Skipped:    0
--------------------------------
Total time: 0.000s
```

`main()` returns the number of failed tests, so a failing test fails your CI too.

A few things to notice:

- `TEST(name)` only defines the test. Nothing runs until you `RUN_TEST(name)` it inside a suite.
- Queued tests run on `SUITE_END()`.
- `CLUT_IMPLEMENTATION` goes in exactly one file. It's what pulls in the implementation.

Tired of keeping `main()` in sync with your tests? Use [runner_generator](runner_generator.md).

## Repeated tests

Same test, N times. Every run gets an `input` with:

```c
input.current_repetition   // 1..N
input.total_repetitions    // N
```

```c
REPEATED_TEST(EvenNumbers, 10) {
  TEST_ASSERT_TRUE(input.current_repetition % 2 == 0);
}
```

Runs all 10 iterations. If any of them fails, the test fails.

### With a threshold

```c
REPEATED_TEST_WITH_THRESHOLD(EvenNumbers, 100, 3) {
  TEST_ASSERT_TRUE(input.current_repetition % 2 == 0);
}
```

Stops as soon as 3 iterations have failed. Good for failing fast.

- fewer than 3 failures: test **passes**
- 3 or more failures: test **fails**

## Parameterized tests

Same test, one run per value. The value is `input`, and the type is whatever you need.

```c
// primitives
PARAM_TEST(IsPrime, int, { 2, 3, 5, 7, 11, 13, 97 }) {
  TEST_ASSERT_TRUE(is_prime(input));
}

// structs
typedef struct { int value; int min; int max; int expected; } ClampCase;

PARAM_TEST(Clamp, ClampCase, {
    {  5, 0, 10,  5 },
    { -3, 0, 10,  0 },
    { 15, 0, 10, 10 },
}) {
  TEST_ASSERT_EQUAL_INT(input.expected, clamp(input.value, input.min, input.max));
}
```

## Custom failure messages

Every assertion has a `_MESSAGE` version. The extra last argument is what gets printed on failure.

```c
TEST_ASSERT_EQUAL_INT_MESSAGE(10, result, "Player score should be initialized correctly");
```

```
[ FAIL ] Score                                         0.000s
game_test.c:18:Score:Player score should be initialized correctly
```

## Output formats

CLUT can print results in three formats. Same two tests as above (`Addition` passes,
`Strings` fails), three different outputs.

### Default

```
[ PASS ] Addition                                      0.000s
[ FAIL ] Strings                                       0.000s
test_strings.c:7:Strings:Expected "Hella" to be equal to "Hello"
--------------------------------
Tests run:  2
Passed:     1
Failed:     1
Skipped:    0
--------------------------------
Total time: 0.000s
```

### GitHub Actions

Failures show up as annotations on the file and line.

```
::notice title=PASS::Addition (0.000s)
::notice title=FAIL::Strings (0.000s)
::error file=test_strings.c,line=7,title=Strings::Expected "Hella" to be equal to "Hello"
::notice title=Suite Results::Tests=2 Passed=1 Failed=1 Skipped=0 Time=0.000s
```

### TAP

```
1..2
ok 1 - Addition # (0.000s)
not ok 2 - Strings # (0.000s)
# test_strings.c:7: Expected "Hella" to be equal to "Hello"
# Tests run: 2, Passed: 1, Failed: 1, Skipped: 0, Time: 0.000s
```
