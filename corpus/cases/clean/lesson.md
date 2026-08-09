# Clean p101 resource handling

This case runs the playground `tour` scenario. It opens files, allocates and
reallocates memory, uses a pipe, forks a child, and cleans everything up.

Expected result: `../scripts/runtime/student-workflow.sh` exits `0` and reports no resource findings.

Use this lab as the reference implementation while fixing the rest of the
series. The point is not that the code is fancy; it is that ownership is easy to
audit.

## What to notice

- Descriptors start as `-1`.
- Pointers start as `NULL`.
- Cleanup is centralized instead of scattered through early returns.
- A descriptor is set back to `-1` after it is closed.
- Fault injection does not reveal a second, hidden cleanup story.

## Fix habit

When a later lab fails, compare it with this one. Most repairs are some version
of: initialize ownership markers, jump to common cleanup, release each resource
once, and clear the marker after release.
