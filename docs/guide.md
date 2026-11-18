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
Tests:      2
Passed:     1
Failed:     1
Skipped:    0
--------------------------------
Total time: 0.000s

================================
RUNNER SUMMARY
================================
Tests:      2
Passed:     1
Failed:     1
Skipped:    0
--------------------------------
Total time: 0.000s
```

Two summaries? One per suite, one for the whole runner. With a single suite they say the
same thing. With ten suites you'll be happy the second one exists.

`main()` returns the number of failed tests (capped at 255), so a failing test fails your CI too.

A few things to notice:

- `TEST(name)` only defines the test. Nothing runs until you `RUN_TEST(name)` it inside a suite.
- Queued tests run on `SUITE_END()`.
- `CLUT_IMPLEMENTATION` goes in exactly one file. It's what pulls in the implementation.
- The first failed assertion stops the test. The line after it never runs.
- Times are CPU time (`clock()`), not wall time. A test that sleeps for a second still says
  `0.000s`.

Tired of keeping `main()` in sync with your tests? Use [runner_generator](runner_generator.md).

## Hooks

Need to set something up before a test and clean it up after? Hooks.

```c
static int counter;

HOOK_BEFORE_EACH(reset_counter) {
  counter = 0;
}

HOOK_AFTER_ALL(say_goodbye) {
  printf("done\n");
}

TEST(Increments) {
  counter++;
  TEST_ASSERT_EQUAL_INT(1, counter);
}

TEST(StartsAtZero) {
  TEST_ASSERT_EQUAL_INT(0, counter);
}

int main() {
  RUNNER_BEGIN();
    SUITE_BEGIN();
      USE_HOOK_BEFORE_EACH(reset_counter);
      USE_HOOK_AFTER_ALL(say_goodbye);
      RUN_TEST(Increments);
      RUN_TEST(StartsAtZero);
    SUITE_END();
  return RUNNER_END();
}
```

Four kinds: `BEFORE_ALL`, `BEFORE_EACH`, `AFTER_EACH`, `AFTER_ALL`. You define with `HOOK_*`,
you plug in with `USE_HOOK_*`, and the names must match.

Hooks belong to a suite. `SUITE_BEGIN()` wipes them, so the next suite starts clean.

Hooks can assert. A broken setup will not silently turn into a green run: what happens for
each hook is in [api-reference.md](api-reference.md#hooks).

## Repeated tests

Same test, N times. Every run gets an `input` with:

```c
input.current_iteration    // 0..N-1
input.total_iterations     // N
```

```c
REPEATED_TEST(EvenNumbers, 10) {
  TEST_ASSERT_TRUE(input.current_iteration % 2 == 0);
}
```

Runs all 10 iterations. If any of them fails, the test fails. Each failure remembers which
iteration it came from:

```
test_numbers.c:12:EvenNumbers[1]:Expected True But Was False -> input.current_iteration % 2 == 0
```

### With a threshold

For tests that are flaky, and you know they are, and you can't fix them today.

```c
REPEATED_TEST_WITH_THRESHOLD(FlakyNetwork, 100, 3) {
  TEST_ASSERT_TRUE(ping_server());
}
```

Runs up to 100 times and stops at the 3rd failed iteration. Fewer than 3 failures and the
test passes. Yes, this hides real failures: it's for flaky tests, not for broken ones. Rules in
[api-reference.md](api-reference.md#how-a-test-ends).

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

A failure tells you which value broke it. The index is the position in your list, so 0-based:

```
test_math.c:8:IsPrime[3]:Expected True But Was False -> is_prime(input)
```

## Skipping tests

```c
TEST(UsesLinuxOnlyAPI) {
#ifndef __linux__
  TEST_SKIP();
#endif
  TEST_ASSERT_TRUE(do_linux_thing());
}
```

Shows up as `[ SKIP ]` and doesn't count as a failure.

In `REPEATED_TEST` and `PARAM_TEST`, skip works per iteration, and the output tells you how
many were skipped (rules in [api-reference.md](api-reference.md#how-a-test-ends)):

```
[ PASS ] IsPrime                                       0.000s (2 skipped)
```

## Floats and doubles

```c
TEST(Float) {
  TEST_ASSERT_EQUAL_FLOAT(0.3f, 0.1f + 0.2f);
}
```

This passes: float equality is relative, with a configurable epsilon. Not enough? Change it:

```c
#define CLUT_FLOAT_EPSILON 1e-3f
#define CLUT_DOUBLE_EPSILON 1e-6
#define CLUT_IMPLEMENTATION
#include "clut.h"
```

One trap: when `expected` is `0`, `actual` must be exactly `0`. For "about zero" use `WITHIN`
(details in [api-reference.md](api-reference.md#numbers-t--char-int-uint-float-double)):

```c
TEST_ASSERT_WITHIN_FLOAT(0.0f, 1e-6f, result);
```

## Custom failure messages

Every assertion has a `_MESSAGE` version. The extra last argument is what you want to say
when it fails.

```c
TEST_ASSERT_EQUAL_INT_MESSAGE(10, result, "Player score should be initialized correctly");
```

```
[ FAIL ] Score                                         0.000s
game_test.c:18:Score:Player score should be initialized correctly: Expected 10 Received 0
```

Your text goes in front, CLUT's message stays after it. You say why it matters, CLUT says
what actually happened.

Booleans and pointers replace the default message instead:

```c
TEST_ASSERT_NOT_NULL_MESSAGE(player, "Player should have been created");
```

```
game_test.c:21:Score:Player should have been created
```

## Output

The default output is what you've been looking at. GitHub Actions, TAP, colors and your own
reporter are in [reporters.md](reporters.md).
