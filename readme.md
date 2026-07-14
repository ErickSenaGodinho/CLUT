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
Tests run:  1
Passed:     1
Failed:     0
--------------------------------
Total time: 0.000s
```

Don't want to write that `main()` by hand? Let `runner_generator` build it for you from your test files:

```sh
runner_generator -o build/test_runner.c tests/test_numbers.h
```

See [docs/runner_generator.md](docs/runner_generator.md) for details.

---

## Why CLUT?

Most C testing frameworks either require extra setup, multiple files or additional tooling.

CLUT keeps things simple:

- Single header integration
- No external dependencies
- Multiple output modes (Default, GitHub Actions, TAP)
- Hooks for test setup and teardown
- Support for repeated and parameterized tests
- Optional CLI tool to auto-generate the test runner from your test files
- Designed for small projects and fast iteration
- CI friendly

---

## Documentation

| Guide | What's in it |
|---|---|
| [docs/guide.md](docs/guide.md) | Repeated tests, parameterized tests, custom assertion messages, output modes |
| [docs/api-reference.md](docs/api-reference.md) | Every macro CLUT exposes: runner/suite, test definitions, hooks, assertions |
| [docs/runner_generator.md](docs/runner_generator.md) | The `runner_generator` CLI tool: what it solves, usage, flags |

---

## Project Structure

```text
CLUT/
├── .github/
├── docs/
│   ├── guide.md
│   ├── api-reference.md
│   └── runner_generator.md/
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
