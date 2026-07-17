# Guide

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
- The test is considered passed if the total number of failures is lower than the configured threshold
- The test is considered failed only when the failure count reaches or exceeds the threshold

---

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

---

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

---

## Output Modes

CLUT can print results in different formats depending on where it's running.

### Default

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
