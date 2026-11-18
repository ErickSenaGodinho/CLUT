# CLUT - C Language Unit Testing Framework

<p align="center">CLUT is a single-header C unit testing framework inspired by JUnit and Unity. It makes writing and running tests simple, with clean, readable output.</p>

<p align="center">
    <img src="https://img.shields.io/github/actions/workflow/status/ErickSenaGodinho/CLUT/ci.yml">
    <img src="https://img.shields.io/github/license/ErickSenaGodinho/CLUT">
</p>

## Basic Usage
```c
#define CLUT_IMPLEMENTATION
#include "clut.h"

TEST(Addition) {
  int result = 2 + 3;
  TEST_ASSERT_EQUAL_INT(5, result);
}

int main() {
  RUNNER_BEGIN();
    SUITE_BEGIN();
      RUN_TEST(Addition);
    SUITE_END();
  return RUNNER_END();
}
```
Output:
```
[ PASS ] Addition                                      0.000s
--------------------------------
Tests:      1
Passed:     1
Failed:     0
Skipped:    0
--------------------------------
Total time: 0.000s

================================
RUNNER SUMMARY
================================
Tests:      1
Passed:     1
Failed:     0
Skipped:    0
--------------------------------
Total time: 0.000s
```

CLUT is header-only, stb style: `#define CLUT_IMPLEMENTATION` before the include, in exactly one file.

Don't want to write that `main()` by hand? Let `runner_generator` build it for you from your test files:

```sh
runner_generator -o build/test_runner.c tests/test_numbers.h
```

See [docs/runner_generator.md](docs/runner_generator.md) for details.

---

## Why CLUT?

Most C testing frameworks either require extra setup, multiple files or additional tooling.

CLUT wants one header. That's it. The example at the top of this page is the whole setup.

Need more? It's there. Don't need it? It stays out of your way.

**Out of the box**

- Single header integration, no external dependencies
- Readable output, ready to go
- CI friendly: exit code is the number of failed tests (max 255)

**When you want more**

- Hooks (`before_all`, `before_each`, `after_each`, `after_all`)
- Repeated and parameterized tests
- Float and double comparison with configurable epsilon
- Multiple output modes (Default, GitHub Actions, TAP), with optional ANSI colors
- Custom reporters
- Everything overridable: allocator, output streams, epsilons, how a test gets aborted
- Optional CLI tool to auto-generate the test runner from your test files

Start simple. Grow when the project does.

---

## Documentation

| Doc | What's in it |
|---|---|
| [docs/guide.md](docs/guide.md) | Examples first: your first test, hooks, repeated and parameterized tests, skipping, floats, custom failure messages |
| [docs/api-reference.md](docs/api-reference.md) | Cheatsheet of every macro: setup, runner/suite, test definitions, hooks, assertions, reporters, configuration |
| [docs/reporters.md](docs/reporters.md) | Output modes, custom reporters, every callback, what you get in each one |
| [docs/runner_generator.md](docs/runner_generator.md) | The `runner_generator` CLI tool: usage, flags, generated runner, include paths |

---

## Project Structure

```text
CLUT/
├── .github/
├── docs/
│   ├── guide.md
│   ├── api-reference.md
│   ├── reporters.md
│   └── runner_generator.md
├── examples/
├── tests/
├── tools/
│   └── runner_generator.c
├── clut.h
├── readme.md
└── LICENSE
```

---

## License

See [LICENSE](LICENSE).
