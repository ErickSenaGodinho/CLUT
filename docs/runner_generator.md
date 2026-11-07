# runner_generator

Writes the `main()` of your test binary for you. No more hand-written
`RUNNER_BEGIN` / `SUITE_BEGIN` / `RUN_TEST` / `SUITE_END` / `RUNNER_END`, and no more
forgetting to add the new test you just wrote.

It's a command-line tool that ships next to CLUT. It is **not** a macro from `clut.h`.
Macro list: [api-reference.md](api-reference.md).

## Usage

```sh
runner_generator -o <output_file> <test_file> [test_file...]
runner_generator -h | --help
```

| Option | What it does |
|---|---|
| `-o, --output <file>` | Where to write the generated runner. Required |
| `-h, --help` | Print usage and exit |

## Example

Two test files:

```c
/* tests/math_test.h */
HOOK_BEFORE_EACH(reset_state) { /* ... */ }

TEST(Addition) {
  TEST_ASSERT_EQUAL_INT(5, 2 + 3);
}
```

```c
/* tests/string_test.h */
TEST(Concat) {
  TEST_ASSERT_EQUAL_STRING("ab", concat("a", "b"));
}
```

Generate, build, run:

```sh
runner_generator -o build/test_runner.c tests/math_test.h tests/string_test.h
gcc build/test_runner.c -o build/test_runner
./build/test_runner
```

`build/test_runner.c` ends up like this:

```c
#define CLUT_IMPLEMENTATION

#include "../tests/math_test.h"
#include "../tests/string_test.h"

int main(void){
	RUNNER_BEGIN();
	SUITE_BEGIN();
		USE_HOOK_BEFORE_EACH(reset_state);
		RUN_TEST(Addition);
	SUITE_END();

	SUITE_BEGIN();
		RUN_TEST(Concat);
	SUITE_END();
	return RUNNER_END();
}
```

The generated file is the one place that defines `CLUT_IMPLEMENTATION`
(see [Setup](api-reference.md#setup)).

## What it picks up

It scans every test file you pass for:

- `TEST`
- `REPEATED_TEST`
- `REPEATED_TEST_WITH_THRESHOLD`
- `PARAM_TEST`
- `HOOK_BEFORE_ALL`, `HOOK_BEFORE_EACH`, `HOOK_AFTER_ALL`, `HOOK_AFTER_EACH`

Then it writes one `.c` file that `#include`s all the test files and registers everything
it found: **one suite per file**, hooks first (as `USE_HOOK_*`), then the `RUN_TEST`s.

## Include paths just work

You run `runner_generator` from one directory, but the generated file gets compiled from
wherever `-o` points, maybe a `build/` folder. Pasting the paths you typed straight into
`#include "..."` would break as soon as the two directories differ.

So it doesn't. It turns the output directory and every test file into absolute paths, then
writes each `#include` **relative to the runner's own location**. That's why the example
above shows `"../tests/math_test.h"` even though you typed `tests/math_test.h`: it figured
out that `build/` sits one level below the project root.

Run it from any directory, point `-o` anywhere, and the includes stay correct.

## Gotchas

- It does **not** create the output directory. The folder you pass to `-o` must already exist.
- If a relative path can't be computed (say the output and a test file are on different
  drives on Windows), it falls back to an absolute path in the `#include`. Ugly, but the
  build still works.
