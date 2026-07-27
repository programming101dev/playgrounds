# Orientation: env, err, wrappers, and tools

This is the first playground case. It is not a bug. It is the map.

Before fixing memory, file, process, IPC, threading, or networking issues, make
sure you can answer four questions:

1. Where is the `struct p101_error` created and destroyed?
2. Where is the `struct p101_env` created and destroyed?
3. Which calls go through `p101_*` wrappers instead of direct libc/POSIX calls?
4. Which tool output proves what happened at runtime?

## Source tour

Start in `src/main.c`.

- `p101_error_create(false)` creates the error object.
- `p101_env_create(err, NULL)` creates the environment.
- The same `env` and `err` are passed into parsing, checking, conversion, and
  the selected scenario.
- `p101_env_destroy(env)` and `p101_error_destroy(err)` release the top-level
  objects.

Then read the `orientation` scenario in `src/playground.c`. It uses a small,
boring pattern on purpose:

```c
resource = p101_wrapper(env, err, ...);
if(resource == NULL || p101_error_has_error(err))
{
    goto done;
}

/* use the resource */

done:
    p101_free(env, resource);
```

That same shape comes back for file descriptors, threads, IPC handles, sockets,
FSM state, and tool event streams.

## Tool tour

Run:

```sh
./tracks/00-p101-orientation/run.sh
```

Then open the generated summary. The important artifacts are:

- wrapper audit: confirms calls did not bypass available p101 wrappers;
- observe output: captures `resources.log` and `calls.log`;
- resource tracker/report: shows allocation and descriptor lifetimes;
- trace output: shows function entry/exit structure;
- error-path walk: shows what happens when p101 calls fail.

## Fix habit

Do not memorize the tools as magic commands. Read their outputs as evidence:

- What resource was acquired?
- Which wrapper acquired it?
- Which function owned it?
- Was it released exactly once?
- Did the same cleanup happen on the error path?

