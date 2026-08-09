# Descriptor leak

This case intentionally leaves an output descriptor open.

Expected result: `../scripts/runtime/student-workflow.sh` exits `1` and the correlated report includes
`P101-FD-001`.

Descriptors are capabilities. If a program keeps one open longer than intended,
it can pin files, exhaust descriptor limits, or accidentally hand access to a
child process later.

## What to look for

- A successful `p101_open`.
- No matching `p101_close` on the same ownership path.
- Cleanup that depends on the happy path reaching the end of the function.

## Fix path

1. Initialize the descriptor variable to `-1`.
2. After `p101_open` succeeds, treat the function as owning that descriptor.
3. Add a cleanup block that closes the descriptor when it is not `-1`.
4. Set the descriptor to `-1` after close.

Re-run `./lab.sh`. This lab is fixed when `P101-FD-001` disappears for
`fd-leak`.
