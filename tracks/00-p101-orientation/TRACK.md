# Track 00: p101 orientation

This is the first playground. It answers the questions students need before
the wrapper-specific tracks make sense:

- What is `struct p101_error`?
- What is `struct p101_env`?
- Why do we use `p101_*` wrappers?
- What do the tools prove?

## Student loop

```sh
./tracks/00-p101-orientation/run.sh
# open /tmp/p101-track-orientation/summary.md
# open the linked corpus and tools reports
```

Use `--full` to keep the HTML reports and bug bundles:

```sh
./tracks/00-p101-orientation/run.sh --full
```

## Lesson sequence

| Step | Command | Purpose |
| ---: | --- | --- |
| 0 | `./corpus.sh --case orientation` | Runs the clean orientation scenario through `p101 check`. |
| 1 | `./corpus.sh --case clean` | Shows the ordinary valid baseline used by later labs. |
| 2 | `./lesson.sh wrappers` | Shows the static wrapper boundary: direct calls can hide behavior from tools. |
| 3 | `./lesson.sh fd-leak` | Shows `p101-observe`, the resource tracker, trace, and report on a visible leak. |
| 4 | `./lesson.sh error-path` | Shows fault injection: fail p101 call N and inspect cleanup behavior. |
| 5 | `./lesson.sh module-split` | Shows the module/API-shape tool before students split larger programs. |

## Source reading order

1. `src/main.c`
   - `p101_error_create(false)`
   - `p101_env_create(err, NULL)`
   - ordinary parse/check/convert/run flow
   - `p101_env_destroy(env)`
   - `p101_error_destroy(err)`
2. `src/playground.c`
   - `run_orientation_demo`
   - `write_text_output`
   - `close_fd_preserving_error`
3. `corpus/cases/orientation/lesson.md`
   - the checked lesson text students see in the lab book

## The shape to copy

```c
resource = p101_wrapper(env, err, ...);
if(resource == NULL || p101_error_has_error(err))
{
    goto done;
}

/* use the resource */

done:
    p101_release_wrapper(env, resource);
    return ret_val;
```

The exact resource changes from heap memory to descriptors, streams, threads,
IPC objects, sockets, FSM state, or parser buffers. The habit does not change:
acquire visibly, check the error object, release in one place, then use the
tools to confirm both the happy path and the error path.

## What students should be able to explain

- Why `env` is passed into p101 wrappers.
- Why fallible wrappers also receive `err`.
- Why direct libc/POSIX calls can make the p101 tools blind.
- What `resources.log` and `calls.log` contain.
- How `p101-report` correlates resource findings with call traces.
- Why `p101-error-path-walk` finds bugs normal happy-path testing misses.

