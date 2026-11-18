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
| `--` | Everything after it is a test file, even if it starts with `-` |

Test files can be wildcards, see [Wildcards](#wildcards).

## Example

Two test files. They include `clut.h` themselves, the generated runner doesn't do it for them:

```c
/* tests/math_test.h */
#include "clut.h"

HOOK_BEFORE_EACH(reset_state) { /* ... */ }

TEST(Addition) {
  TEST_ASSERT_EQUAL_INT(5, 2 + 3);
}
```

```c
/* tests/string_test.h */
#include "clut.h"

TEST(Concat) {
  TEST_ASSERT_EQUAL_STRING("ab", concat("a", "b"));
}
```

Generate, build, run:

```sh
runner_generator -o build/test_runner.c tests/math_test.h tests/string_test.h
gcc -I. build/test_runner.c -o build/test_runner
./build/test_runner
```

`-I.` is there because `clut.h` sits in the project root and the test files live in `tests/`.
Put it wherever your compiler can find it.

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
(see [Setup](api-reference.md#setup)). It's defined before the first `#include`, so the first
`clut.h` that gets pulled in brings the implementation with it.

## What it picks up

It scans every test file you pass for:

- `TEST`
- `REPEATED_TEST`
- `REPEATED_TEST_WITH_THRESHOLD`
- `PARAM_TEST`
- `HOOK_BEFORE_ALL`, `HOOK_BEFORE_EACH`, `HOOK_AFTER_ALL`, `HOOK_AFTER_EACH`
- `REPORTER`

Then it writes one `.c` file that `#include`s all the test files and registers everything
it found: **one suite per file**, hooks first (as `USE_HOOK_*`), then the `RUN_TEST`s.

What it looks at, and what it ignores:

- Only top-level macros count. A `TEST(...)` inside a function body or any other `{ }` is ignored.
- Comments, string literals and preprocessor lines (`#define`, ...) are skipped. A commented-out
  test stays commented out, and a `TEST(...)` hidden in a `#define` isn't registered.
- The first thing after the `(` has to be the name. The rest of the arguments don't matter to it.

## Reporters

Defined a custom reporter with `REPORTER(name, ...)` in one of your test files? The generator
sees it and puts `USE_REPORTER(name);` in `main()`, before `RUNNER_BEGIN()`:

```c
int main(void){
	USE_REPORTER(dots);
	RUNNER_BEGIN();
	...
```

How to write one: [reporters.md](reporters.md). A few things to know:

- It scans all the files you pass. The reporter can live in a file with no tests at all.
- Two reporters, two `USE_REPORTER`s. The last one wins.

## Configuration

The generated file is just a `main()`, so there's nowhere to put a `#define`. Pass it on the
compile line:

```sh
gcc -I. -DCLUT_OUTPUT_TAP build/test_runner.c -o build/test_runner
```

Any define from [Configuration](api-reference.md#configuration) works the same way.

## Wildcards

Don't want to list every file?

```sh
runner_generator -o build/test_runner.c "tests/*_test.h"
```

`*` and `?` are expanded by the tool itself. That matters on Windows, where the shell won't do
it for you. On Linux and macOS the shell expands unquoted wildcards first, which works too.

- Only the file name part can have a wildcard. `tests/*_test.h` works, `tests/*/x.h` doesn't.
- The matched files are **not sorted**. They come in whatever order the filesystem returns
  them, so don't count on the suite order. Need a fixed order? List the files by hand.
- No match: it prints a warning and goes on.

## Include paths just work

You run `runner_generator` from one directory, but the generated file gets compiled from
wherever `-o` points, maybe a `build/` folder. Pasting the paths you typed straight into
`#include "..."` would break as soon as the two directories differ.

So it doesn't. It turns the output directory and every test file into absolute paths, then
writes each `#include` **relative to the runner's own location**. That's why the example
above shows `"../tests/math_test.h"` even though you typed `tests/math_test.h`: it figured
out that `build/` sits one level below the project root.

Run it from any directory, point `-o` anywhere, and the includes stay correct.

## When something goes wrong

A test file it can't read, or a macro it can't parse (`TEST` without `(`, a name that isn't an
identifier, a `(` that never closes), and it stops:

```
tests/math_test.h:12: error: expected ...
```

Exit code 1, and **no runner is written**. The output file you already had is left exactly as
it was, so a broken test file never leaves you with a half-written `main()`.

## Gotchas

- It does **not** create the output directory. The folder you pass to `-o` must already exist.
- A file with hooks but no tests gets no suite, so its hooks are never used.
- Two hooks of the same kind in one file: both get a `USE_HOOK_*`, and the last one wins.
- If a relative path can't be computed (say the output and a test file are on different
  drives on Windows), it falls back to an absolute path in the `#include`. Ugly, but the
  build still works.
- If it can't resolve the absolute path of the output at all, it warns you and writes the
  `#include`s exactly as you typed them. Then it's on you to make them valid from where the
  runner is compiled.
