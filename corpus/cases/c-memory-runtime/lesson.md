# Track smoke: C memory runtime wrappers

This case is not a broken-code lab. It is a coverage smoke for the first
playground track.

The `c-memory-runtime` scenario exercises every wrapper assigned to Track 01:
allocation, `realloc`, numeric helpers, multibyte conversion, parsing,
environment variables, temporary files/directories, random helpers, pty helpers,
process termination wrappers, and safe command execution.

Expected result: `p101 check` exits `0`, reports no findings, and the output
contains:

```text
c-memory-runtime: exercised 57 wrappers
```

## Why this exists

The track has 57 wrappers, but only some of them are natural broken-code labs.
This smoke test proves the whole wrapper family still compiles, links, runs, and
is visible to the p101 observation stack.

## Fix habit

If this smoke fails, do not remove the wrapper from the track. Fix the wrapper
use, portability assumption, feature-test macro, or library link so the first
track remains complete.
