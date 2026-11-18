# Reporters

A reporter decides what CLUT prints. That's all it does. CLUT runs the tests, the reporter
tells you about it.

Quick map: [api-reference.md](api-reference.md) has every macro, [guide.md](guide.md) has the
basics. This one is about output.

---

## Built-in output modes

Three of them. You pick one at compile time, before including `clut.h`:

```c
#define CLUT_OUTPUT_GITHUB    // GitHub Actions annotations
#define CLUT_OUTPUT_TAP       // TAP
// neither: default human-readable output

#define CLUT_IMPLEMENTATION
#include "clut.h"
```

If you define both, GitHub wins. Don't define both.

Same two tests in every example: `Addition` passes, `Strings` fails.

**Default**

The one you see in [guide.md](guide.md). Failures go to `CLUT_STREAM_FAIL` (stderr),
everything else to `CLUT_STREAM_DEFAULT` (stdout). Want colors? Add `CLUT_OUTPUT_COLOR`.
It only affects the default mode. GitHub and TAP don't care about your terminal.

**GitHub Actions**

Prints `::notice` and `::error` lines. GitHub turns them into annotations on the exact file
and line that failed. No parsing on your side.

```
::notice title=PASS::Addition (0.000s)
::notice title=FAIL::Strings (0.000s)
::error file=test_strings.c,line=7,title=Strings::Expected "Hella" to be equal to "Hello"
::notice title=Suite Summary::Tests=2 Passed=1 Failed=1 Skipped=0 Time=0.000s

::notice title=Runner Summary::Total Tests=2 Passed=1 Failed=1 Skipped=0 Time=0.000s

```

**TAP**

The plan (`1..N`) comes at the end of the suite, which TAP allows. Skipped tests are
`ok N - name # SKIP`. The numbering restarts on every suite, so with several suites you get
several plans. Feed it to whatever TAP consumer you already have.

```
ok 1 - Addition # (0.000s)
not ok 2 - Strings # (0.000s)
# test_strings.c:7: Expected "Hella" to be equal to "Hello"
1..2
# Suite Summary: Tests: 2, Passed: 1, Failed: 1, Skipped: 0, Time: 0.000s

# Runner Summary: Tests: 2, Passed: 1, Failed: 1, Skipped: 0, Time: 0.000s

```

---

## Custom reporters

Not happy with any of the above? Write your own. A reporter is this struct:

```c
typedef struct {
  void (*on_runner_start)(void *context);
  void (*on_test_start)(void *context, const char *test_name);
  void (*on_test_pass)(void *context, const ClutTestResult *result);
  void (*on_test_fail)(void *context, const ClutTestResult *result);
  void (*on_test_skip)(void *context, const ClutTestResult *result);
  void (*on_suite_end)(void *context, const ClutSummary *summary);
  void (*on_runner_end)(void *context, const ClutSummary *summary);

  void *context;
} ClutReporter;
```

Every callback is optional. `NULL` means CLUT doesn't call it. `context` is yours: whatever
pointer you put there comes back as the first argument of every callback.

Define one with `REPORTER`, turn it on with `USE_REPORTER`:

```c
#define CLUT_IMPLEMENTATION
#include "clut.h"
#include <stdio.h>

static void dots_pass(void *context, const ClutTestResult *result) {
  (void)context; (void)result;
  putchar('.');
}

static void dots_fail(void *context, const ClutTestResult *result) {
  (void)context; (void)result;
  putchar('F');
}

static void dots_skip(void *context, const ClutTestResult *result) {
  (void)context; (void)result;
  putchar('S');
}

static void dots_runner_end(void *context, const ClutSummary *summary) {
  (void)context;
  printf("\n%zu/%zu passed\n", summary->passed, summary->total_tests);
}

REPORTER(dots,
  .on_test_pass  = dots_pass,
  .on_test_fail  = dots_fail,
  .on_test_skip  = dots_skip,
  .on_runner_end = dots_runner_end
);

int main() {
  USE_REPORTER(dots);
  RUNNER_BEGIN();
    SUITE_BEGIN();
      RUN_TEST(Addition);
    SUITE_END();
  return RUNNER_END();
}
```

Output:

```
...F.S
5/6 passed
```

`USE_REPORTER` goes **before** `RUNNER_BEGIN()`. `RUNNER_BEGIN()` is the one that calls
`on_runner_start`, so if you set the reporter after it, that callback never fires.

A custom reporter replaces the built-in one completely. Nothing gets printed unless you print it.

### Using `context`

Need state? Don't reach for a global. Hand a pointer to the reporter:

```c
typedef struct {
  size_t failed_assertions;
} MyStats;

static MyStats stats = {0};

static void stats_fail(void *context, const ClutTestResult *result) {
  MyStats *s = (MyStats *)context;
  s->failed_assertions += result->failure_count;

  for (size_t i = 0; i < result->failure_count; ++i) {
    const ClutTestFailure *f = &result->failures[i];
    fprintf(stderr, "%s:%d: %s\n", f->file, f->line, f->message);
  }
}

REPORTER(stats_reporter,
  .on_test_fail = stats_fail,
  .context      = &stats
);
```

---

## When each callback fires

| Callback | When |
|---|---|
| `on_runner_start` | `RUNNER_BEGIN()` |
| `on_test_start` | Right before a test starts, before its `before_each` hook. Once per test, not once per iteration |
| `on_test_pass` | Test finished, nothing failed |
| `on_test_fail` | Test finished, something failed. Also fires for a failing `before_all` / `after_all` hook |
| `on_test_skip` | Test skipped with `TEST_SKIP()`, or never ran because `before_all` failed |
| `on_suite_end` | `SUITE_END()`, after `after_all` |
| `on_runner_end` | `RUNNER_END()` |

A suite with zero tests is not run at all. No hooks, no `on_suite_end`.

## What you get

```c
typedef struct {
  const char *test_name;
  const ClutTestFailure *failures;   // array, failure_count entries
  size_t failure_count;
  double duration;                   // CPU seconds (clock()), summed over all iterations
  size_t skipped_iterations;         // REPEATED_TEST / PARAM_TEST only
} ClutTestResult;

typedef struct {
  const char *file;
  int line;
  int iteration;                     // -1 for a plain TEST
  const char *message;
  size_t message_offset;             // internal, ignore it
} ClutTestFailure;

typedef struct {
  size_t total_tests;
  size_t passed;
  size_t failed;
  size_t skipped;
  double total_seconds;
} ClutSummary;
```

Things to know:

- **Pointers die with the callback.** `result`, `failures`, `message`: CLUT reuses those
  buffers for the next test. Need something later? Copy it.
- **`iteration`** is 0-based, in `REPEATED_TEST*` and in `PARAM_TEST`. It matches
  `input.current_iteration` for the first and the array index for the second.
  `-1` means a plain `TEST`, where there are no iterations.
- **Skipped by a failed `before_all`**: you get `on_test_skip` with no failures and no
  `on_test_start`. The test never started.
- **Hook failures** show up as a test named `before_all` or `after_all`.

---

## Where the output goes

```c
#define CLUT_STREAM_DEFAULT stdout   // everything that isn't a failure
#define CLUT_STREAM_FAIL    stderr   // failures
```

Redefine before including `clut.h` if you want a file or a different stream:

```c
#define CLUT_STREAM_DEFAULT log_file
```

Only the built-in reporters use these. Your own reporter prints wherever it wants.
