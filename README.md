# p101-tool-playground

`p101-tool-playground` is a full-featured demo target for the Programming 101
runtime tools. It deliberately exercises files, heap allocations, realloc,
pipes, forked children, clean cleanup, intentional leaks, bad closes, fault
injection, tests, fuzzing, and coverage.

This repository is the single playground home for the wrapper curriculum. It is
organized as many small tracks rather than one broad `systems` bucket or a pile
of standalone example repos. See [tracks/README.md](./tracks/README.md) for the
generated track map.

Start with the orientation pre-track:

```sh
./tracks/00-p101-orientation/run.sh
```

That first pass introduces `struct p101_error`, `struct p101_env`, p101
wrappers, and the tools before the wrapper-family tracks begin. Every track
directory is also a standalone mini-project with its own `src/`, `include/`,
`config.cmake`, `test/`, and `run.sh`.

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

## Track map

The track map is generated from the library function graph, so it stays tied to
the actual wrapper surface:

- 40 tracks;
- 1 orientation pre-track;
- 1,185 wrappers assigned to one primary track;
- 0 uncovered domains;
- no `misc` bucket.

Refresh it after changing wrapper libraries or classification rules:

```sh
../scripts/analyze-lib-function-graph.py
./sync-track-map.py
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
| `orientation` | First p101 tour: env, err, wrappers, and tools | clean |
| `c-memory-runtime` | Clean smoke test for all 55 Track 01 wrappers | clean |
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
| `unsafe-log-secret` | Log a secret directly | unsafe logging bug |
| `log-injection` | Allow untrusted text to forge a log line | log-forgery bug |
| `missing-structured-log` | Emit vague text instead of event fields | logging practice bug |
| `input-validation` | Accept a path-shaped input value | validation bug |
| `command-injection` | Build a shell command from untrusted text | command-construction bug |
| `predictable-temp-file` | Use a predictable file in `/tmp` | temporary-file bug |
| `signed-conversion` | Convert a negative count to an unsigned size | integer-conversion bug |
| `truncation` | Store a large value in a too-small type | truncation bug |
| `use-after-free` | Continue using ownership after free | lifetime bug |
| `realloc-failure` | Lose the original pointer when growth fails | cleanup/ownership bug |
| `cleanup-order` | Destroy parent state before child resources | cleanup-order bug |
| `partial-write` | Treat a short write as complete | I/O bug |
| `interrupted-syscall` | Treat `EINTR` as a final failure | retry bug |
| `string-not-terminated` | Treat bytes without a NUL terminator as a C string | string bug |
| `out-of-bounds-write` | Accept a write past the buffer end | bounds bug |
| `out-of-bounds-read` | Accept a read past valid data | bounds/information bug |
| `buffer-overflow` | Copy more bytes than the destination holds | buffer overflow bug |
| `uninitialized-read` | Use a value before initialization | state bug |
| `null-dereference` | Continue after a NULL result | checked-result bug |
| `integer-overflow` | Let a size calculation wrap | integer/allocation bug |
| `unchecked-parse` | Accept trailing junk after a numeric parse | parsing bug |
| `path-traversal` | Accept a path that escapes its root | path validation bug |
| `trusted-environment` | Trust environment variables for sensitive behavior | environment bug |
| `missing-authorization` | Authenticate a user but skip the permission check | authorization bug |
| `short-read` | Treat a short read as a complete object | I/O bug |
| `read-eof-handling` | Confuse EOF with an I/O error | I/O bug |
| `format-string` | Treat user text as a format string | format-string bug |
| `unsafe-file-mode` | Create a sensitive file with broad permissions | file-permission bug |
| `symlink-follow` | Follow an untrusted symlink for a sensitive open | file/link bug |
| `stale-secret` | Reuse a buffer without clearing old secret data | information exposure bug |
| `resource-exhaustion` | Accept an unbounded resource request | resource-limit bug |
| `toctou` | Check a path separately from the later use | race bug |
| `data-race` | Update shared state without synchronization | concurrency bug |
| `thread-argument-lifetime` | Let a thread outlive its argument storage | concurrency/lifetime bug |
| `parser-fuzz` | Parse boundary-heavy input without fuzz coverage | testing practice bug |

The bug scenarios are intentionally broken. They are not regressions; they are
teaching targets.

## Lab progression

The lab book orders cases as a progression for students who already know basic C
syntax but have not yet practiced defensive systems programming:

1. baseline and resource ownership;
2. normal cleanup mistakes;
3. memory lifetime mistakes;
4. error-path cleanup;
5. checked results, initialization, read/write byte counts, strings, bounds, and integer traps;
6. adversarial input, authorization, environment, and file-security mistakes;
7. logging and observability practice;
8. concurrency/race hazards;
9. parser fuzzing and verification practice.

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
./lesson.sh all
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
./lab.sh --track c
./lab.sh --track systems
./lab.sh --track network
```

The lab generator runs the same checked corpus and writes a self-contained
`index.html` plus `lab.md`. It is a series, not a single worksheet: each issue
has an ordered lab ID, a dedicated `lesson.md`, a fix checklist, and a progress
state. Students fix one issue, re-run `./lab.sh`, and watch that lab move from
`OPEN` to `FIXED`.

The intended student loop is:

```sh
./lab.sh
# choose the first OPEN lab
# edit the matching scenario in src/playground.c
./build.sh
./lab.sh
```

If an experiment goes sideways, preview the reset with `./reset-labs.sh --show`
or restore the committed fixtures with `./reset-labs.sh --yes`. Before
submission, run:

```sh
./submit-labs.sh
```

Networking-specific labs live in this repository too, as focused networking
tracks rather than a separate playground repo. TCP, UDP, interfaces, socket
nonblocking behavior, protocol framing, and the port-forwarder capstone should
land under the relevant `network-*` track directories as those lessons are
filled in.

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
  -r ../programs/p101-resource-tracker/build-clang/p101-resource-tracker \
  -t ../programs/p101-trace/build-clang/p101-trace \
  -p ../programs/p101-report/build-clang/p101-report \
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
  -r ../programs/p101-resource-tracker/build-clang/p101-resource-tracker \
  -p ../programs/p101-report/build-clang/p101-report \
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
