# Early return leaks a descriptor

This case opens a file descriptor, allocates a buffer, writes the buffer, frees
the allocation, and then returns without closing the descriptor.

That bug is small enough to miss in code review because the happy path looks
linear. The problem is ownership: once `p101_open` succeeds, the function owns a
descriptor until `p101_close` releases it.

## What to look for

- A successful `p101_open`.
- A later `return` before the matching `p101_close`.
- Cleanup spread across several branches instead of one obvious exit path.

## Fix path

Use the same shape as the clean tour:

1. Initialize the descriptor to `-1`.
2. Use `goto done` or another common cleanup path after acquisition.
3. In cleanup, close only when the descriptor is not `-1`.
4. Set the descriptor back to `-1` after a successful close.

Re-run `./lab.sh`. This lab is fixed when `P101-FD-001` disappears for
`early-return-fd-leak`.
