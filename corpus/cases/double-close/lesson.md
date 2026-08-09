# Double close

This case closes one descriptor twice.

Expected result: `../scripts/runtime/student-workflow.sh` exits `1` and the correlated report includes
`P101-FD-002`.

Double release bugs are ownership bugs. With descriptors, a second close can be
especially nasty because the integer may have been reused for a different file
or socket by the time cleanup runs again.

## What to look for

- Two `p101_close` calls that can receive the same descriptor value.
- Cleanup paths that both believe they own the same resource.
- A missing `fd = -1` after the first successful close.

## Fix path

1. Choose one owner for the descriptor.
2. Close it in one place.
3. Immediately clear the descriptor variable to `-1`.
4. Guard cleanup with `if(fd != -1)`.

Re-run `./lab.sh`. This lab is fixed when `P101-FD-002` disappears for
`double-close`.
