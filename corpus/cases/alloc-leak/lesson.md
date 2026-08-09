# Allocation leak

This case intentionally leaves one heap allocation live at process exit.

Expected result: `../scripts/runtime/student-workflow.sh` exits `1` and the correlated report includes
`P101-ALLOC-001`.

Heap leaks are not just “messy.” In long-running programs they become
availability bugs: each request or connection can permanently consume a little
more memory.

## What to look for

- A `p101_malloc`, `p101_calloc`, or `p101_realloc` result.
- A success path that never calls `p101_free`.
- Ownership that is implicit instead of written into the control flow.

## Fix path

1. Decide which function owns the pointer.
2. Initialize the pointer to `NULL`.
3. Free it in a common cleanup path.
4. If ownership moves somewhere else, make that transfer obvious.

Re-run `./lab.sh`. This lab is fixed when `P101-ALLOC-001` disappears for
`alloc-leak`.
