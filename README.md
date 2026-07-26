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
| `stray-close` | Close an unopened descriptor | bad release |
| `fault-lab` | Clean normally, leaky when setup calls are fault-injected | error-path leaks |
| `early-return-fd-leak` | Return before descriptor cleanup | descriptor leak |
| `early-return-alloc-leak` | Return before allocation cleanup | allocation leak |
| `partial-cleanup` | Acquire several resources and release only some | descriptor + allocation leaks |
| `realloc-leak` | Grow an allocation and forget to free the result | allocation leak |
| `exec-inherit` | Reach exec with a descriptor lacking `FD_CLOEXEC` | exec inheritance |
| `double-free` | Free one allocation twice | double free |
| `stray-free` | Free an interior pointer | free of unknown pointer |
| `sizeof-pointer` | Use `sizeof(pointer)` as though it were buffer length | logic/output-size bug |
| `ignore-read-count` | Write a whole buffer instead of bytes actually read | stale-data output bug |

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

For shorter classroom slices, use the lesson script:

```sh
./lesson.sh wrappers
./lesson.sh fd-leak
./lesson.sh error-path
./lesson.sh module-split
```

For the regression/lesson corpus, run:

```sh
./corpus.sh --quick
./corpus.sh
```

The corpus lives under `corpus/cases/`. Each case has an `expected.json` oracle
and a short `lesson.md`. The runner executes each scenario through `p101 check`,
verifies expected exit status and diagnostic IDs, and writes a linked
`summary.md` plus the full per-case HTML reports. This makes the playground both
a demo target and the checked answer key for the toolchain.

New lessons should be mapped to the source-of-truth checklist in
`corpus/CANONICAL-SOURCES.md`, which cross-references CERT C, CWE, ISO C secure
coding rules, MISRA/JPL safer-C guidance, the classic comp.lang.c FAQ, OWASP
logging guidance, and NIST secure-development practices. The checklist covers
both broken-code traps and positive practices such as structured logging,
consistent error handling, input validation, narrow module scope, testing,
fuzzing, coverage, and reproducible bug bundles.

For the student-facing lab series, run:

```sh
./lab.sh --quick
./lab.sh
```

The lab generator runs the same checked corpus and writes a self-contained
`index.html` plus `lab.md`. It is a series, not a single worksheet: each issue
has an ordered lab ID, a dedicated `lesson.md`, a fix checklist, and a progress
state. Students fix one issue, re-run `./lab.sh`, and watch that lab move from
`OPEN` to `FIXED`.

For instructor/CI checks that prove the committed broken fixtures still produce
their expected diagnostics, add `--strict-corpus`:

```sh
./lab.sh --strict-corpus
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

## Boundaries

`p101-tool-playground` is a curated demonstration target. Its scenarios are
designed to exercise the p101 tools clearly; they are not a substitute for
running the tools on a student's actual project. Bug scenarios are intentionally
broken teaching fixtures, and a clean playground tour only proves that this demo
scenario stayed inside its expected resource model.

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

Or run `./lab.sh` first and use the generated `index.html` as the table of
contents for the whole lesson.
