# Track 01: C memory runtime

This is the first playground track. It teaches the ownership grammar used by
the rest of the p101 curriculum: create the environment and error object,
allocate deliberately, release exactly once, preserve ownership through
`realloc`, and keep error paths boring.

## Why this track comes first

File descriptors, threads, IPC objects, sockets, logs, and FSM state all use the
same underlying habit: know who owns the resource, make cleanup visible, and
test the error path. Heap memory is the smallest place to learn that habit.

## Student loop

```sh
./tracks/01-c-memory-runtime/run.sh
# read the first OPEN case
# edit the matching scenario in src/playground.c
./build.sh
./tracks/01-c-memory-runtime/run.sh
```

The runner writes its report to `/tmp/p101-track-c-memory-runtime` by default.

## Lesson sequence

| Step | Case | Scenario | Purpose |
| ---: | --- | --- | --- |
| 0 | `clean` | `tour` | Valid baseline: env/error setup, owned pointers, descriptors, common cleanup, and clean fault injection. |
| 1 | `c-memory-runtime` | `c-memory-runtime` | Clean smoke test that exercises all 55 wrappers assigned to this track. |
| 2 | `alloc-leak` | `alloc-leak` | Every successful allocation needs a visible owner and release path. |
| 3 | `early-return-alloc-leak` | `early-return-alloc-leak` | Early returns after acquisition should still pass through cleanup. |
| 4 | `realloc-leak` | `realloc-leak` | A successful realloc returns the still-owned final pointer. |
| 5 | `realloc-failure` | `realloc-failure` | Store realloc in a temporary so failure does not lose the original allocation. |
| 6 | `double-free` | `double-free` | A resource should have one release path; clear or transfer ownership explicitly. |
| 7 | `stray-free` | `stray-free` | Only free the exact pointer returned by the allocator. |
| 8 | `use-after-free` | `use-after-free` | Do not keep using a pointer after ownership has been released. |
| 9 | `resource-exhaustion` | `resource-exhaustion` | Bound resource requests before allocation or loops. |

## Valid shape to copy

The valid shape is deliberately plain:

```c
char *buffer = NULL;
int   rc     = EXIT_FAILURE;

buffer = p101_malloc(env, err, bytes);
if(buffer == NULL || p101_error_has_error(err))
{
    goto done;
}

/* use buffer */
rc = EXIT_SUCCESS;

done:
    p101_free(env, buffer);
    return rc;
```

That pattern is not glamorous. That is the point. It scales to descriptors,
threads, IPC handles, sockets, FSM state, and tool event streams.

## What students should be able to explain

- Why pointers start as `NULL`.
- Why cleanup happens in one place.
- Why `p101_realloc` should usually write to a temporary pointer first.
- Why double free and stray free are security-shaped bugs, not just style bugs.
- Why a resource limit is part of memory safety.
- How `p101-observe`, `p101-resource-tracker`, and `p101-error-path-walk` make
  the hidden paths visible.

## Wrapper families touched

This track is centered on the `c-memory-runtime` wrapper family:

- allocation: `p101_malloc`, `p101_calloc`, `p101_realloc`, `p101_free`;
- process/runtime: `p101_exit`, `p101_exit_immediately`, `p101_abort`,
  `p101_atexit`, `p101_getenv`, `p101_system`;
- conversion/runtime helpers from `stdlib` and common extensions.
