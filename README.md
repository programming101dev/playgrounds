# p101-tool-playground

`p101-tool-playground` is a full-featured demo target for the Programming 101
runtime tools. It deliberately exercises files, heap allocations, realloc,
pipes, forked children, clean cleanup, intentional leaks, bad closes, fault
injection, tests, fuzzing, and coverage.

The point is to have one program that makes the whole toolchain visible:

```text
p101-tool-playground
  -> p101-observe
      -> resources.log
      -> calls.log
      -> p101-resource-tracker
      -> p101-trace
      -> p101-report
  -> p101-error-path-walk
      -> fail p101 call N
      -> per-run resource/call/fault/report artifacts
```

## Usage

```sh
p101-tool-playground [-h] [-v] [-s <scenario>] [-o <output-path>] [-b <bytes>] [-r <repeats>]
```

Options:

- `-s <scenario>` chooses what to demonstrate. Default: `tour`.
- `-o <path>` chooses the output file used by file scenarios.
- `-b <bytes>` controls allocation/write size, from `1` to `4096`.
- `-r <repeats>` controls repeated pipe/file operations, from `1` to `32`.
- `-v` enables p101 tracing inside the playground itself.

## Scenarios

| Scenario | Purpose | Expected resource result |
| --- | --- | --- |
| `tour` | Clean file, realloc, pipe, and fork path | clean |
| `clean-file` | Open/write/close/free | clean |
| `realloc` | Allocate, grow, and free | clean |
| `pipe` | Create/read/write/close pipe fds | clean |
| `fork` | Child allocates and writes through inherited pipe | clean |
| `fd-leak` | Intentionally leave one descriptor open | descriptor leak |
| `alloc-leak` | Intentionally leave one allocation live | allocation leak |
| `double-close` | Close a descriptor twice | bad release |
| `stray-close` | Close descriptor `-1` | bad release |
| `fault-lab` | Clean normally, leaky when setup calls are fault-injected | error-path leaks |

The bug scenarios are intentionally broken. They are not regressions; they are
teaching targets.

## Full toolchain tour

Run the one-command tour:

```sh
./tour.sh
```

That writes a timestamped report directory under `/tmp` with strict checks,
tests, fuzzing, coverage, observed resource/call reports, correlated reports,
and a fault-injected error-path walk. For quick runtime-only demos:

```sh
./tour.sh --skip-quality --skip-coverage -n 5
```

Build the playground:

```sh
./change-compiler.sh -c clang
./build.sh
```

Run a clean observed tour:

```sh
p101-observe \
  -o /tmp/p101-playground-tour \
  -r ../p101-resource-tracker/build-clang/p101-resource-tracker \
  -t ../p101-trace/build-clang/p101-trace \
  -p ../p101-report/build-clang/p101-report \
  -- ./build-clang/p101-tool-playground -s tour
```

Run a descriptor leak:

```sh
p101-observe \
  -o /tmp/p101-playground-fd-leak \
  -- ./build-clang/p101-tool-playground -s fd-leak
```

Inspect individual artifacts:

```sh
p101-resource-tracker /tmp/p101-playground-fd-leak/resources.log
p101-resource-tracker -j /tmp/p101-playground-fd-leak/resources.log
p101-trace /tmp/p101-playground-fd-leak/calls.log
p101-report /tmp/p101-playground-fd-leak
p101-report -j /tmp/p101-playground-fd-leak
```

Walk injected error paths:

```sh
p101-error-path-walk \
  -n 20 \
  -l /tmp/p101-playground-walk \
  -r ../p101-resource-tracker/build-clang/p101-resource-tracker \
  -p ../p101-report/build-clang/p101-report \
  -- ./build-clang/p101-tool-playground -s fault-lab
```

The `fault-lab` scenario is clean without injection. Its cleanup becomes
intentionally sloppy only after a p101 setup call fails, so
`p101-error-path-walk` can show exactly which injected failures leak resources.

## Testing, fuzzing, and coverage

The playground also demonstrates the project pipeline itself:

```sh
./test.sh
./fuzz.sh -t 10
./change-compiler.sh -c clang --coverage
./build.sh
./test.sh --coverage
./coverage-report.sh --no-open --min 50 -- -s tour
./check.sh
```

- Tests cover argument parsing, scenario lookup, and validation.
- The fuzz target pounds the command-line parser.
- Coverage shows students what the tests and an instrumented demo run actually
  execute.
- `check.sh` ties format, strict build, tests, and fuzz smoke together.

## Suggested classroom flow

1. Run `tour` with `p101-observe` and inspect `summary.txt`.
2. Run `fd-leak` and compare `resource-report.txt` with
   `correlated-report.txt`.
3. Run `alloc-leak` and inspect `correlated-report.json`.
4. Run `p101-trace` directly on `calls.log`.
5. Run `fault-lab` through `p101-error-path-walk` and find the first injected
   failure that leaks.
6. Run `./test.sh`, `./fuzz.sh`, and `./coverage-report.sh` to show the static
   and dynamic quality workflow around the demo.
