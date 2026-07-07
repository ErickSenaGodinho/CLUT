# runner_generator

A command-line tool that builds the `main()` test runner for you, so you don't have to
hand-write `RUNNER_BEGIN` / `SUITE_BEGIN` / `RUN_TEST` / `SUITE_END` / `RUNNER_END` and
keep it in sync every time you add a test file.

## What it does

Given one or more test files, `runner_generator`:

1. Scans each file for `TEST`, `REPEATED_TEST`, `REPEATED_TEST_WITH_THRESHOLD`,
   `PARAM_TEST`, and hook macros (`BEFORE_ALL_HOOK`, `BEFORE_EACH_HOOK`,
   `AFTER_ALL_HOOK`, `AFTER_EACH_HOOK`).
2. Generates a single `.c` file that `#include`s every test file and wires up a
   `main()` that registers every test/hook it found, in a single suite per file.

## Usage

```sh
runner_generator -o <output_file> <test_file> [test_file...]
runner_generator -h | --help
```

| Option | Description |
|---|---|
| `-o, --output <file>` | Path of the generated runner (required) |
| `-h, --help` | Show usage and exit |

### Example

```sh
runner_generator -o build/test_runner.c tests/math_test.h tests/string_test.h
gcc build/test_runner.c -o build/test_runner
./build/test_runner
```

`build/test_runner.c` will look like:

```c
#define CLUT_IMPLEMENTATION

#include "../tests/math_test.h"
#include "../tests/string_test.h"

int main(void){
	RUNNER_BEGIN();
	SUITE_BEGIN();
		SET_BEFORE_EACH(reset_state);
		RUN_TEST(Addition);
	SUITE_END();

	SUITE_BEGIN();
		RUN_TEST(Concat);
	SUITE_END();
	return RUNNER_END();
}
```

## Include paths are resolved for you

Test file paths on the command line are relative to wherever you *ran* `runner_generator`
from, but the generated file gets compiled from wherever `-o` points to, which can be a
completely different directory (e.g. a `build/` folder). If the tool just copied the paths
you typed verbatim into `#include "..."`, they could easily point at the wrong file, or
nowhere at all - once the runner is built somewhere else.

`runner_generator` avoids this by resolving both the output directory and every test file
to an absolute path, then computing each `#include` relative to *the runner's own
location* instead of your current directory. That's why the example above shows
`"../tests/math_test.c"` even though the CLI was invoked with a bare `tests/math_test.c` -
the tool worked out that `build/` sits one level below the project root.

This means you can call `runner_generator` from any directory, point `-o` anywhere, and the
generated file will always `#include` the right files.

## Notes

- `runner_generator` does not create missing output directories - the folder passed to
  `-o` must already exist.
- If a relative path genuinely can't be computed (e.g. the output and a test file live on
  different drives on Windows), the tool falls back to an absolute path in the
  `#include`, so the build still works - it just won't be a pretty relative path.
