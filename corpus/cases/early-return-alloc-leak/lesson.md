# Early return leaks an allocation

This case allocates a buffer and returns success while the pointer is still
owned by the function.

The security habit is the same as the correctness habit: make ownership boring.
Every pointer returned by `p101_malloc` should have an obvious release site.
When a function has several exits, the release site is easiest to audit when it
is shared.

## What to look for

- A `p101_malloc` or helper that returns allocated memory.
- A success return before `p101_free`.
- Code that would become worse if one more error check were added later.

## Fix path

1. Start `buffer` as `NULL`.
2. Jump to one cleanup block for all exits after allocation.
3. Call `p101_free(env, buffer)` in that cleanup block.
4. Keep success/failure status in a variable instead of returning directly.

Re-run `./lab.sh`. This lab is fixed when `P101-ALLOC-001` disappears for
`early-return-alloc-leak`.
