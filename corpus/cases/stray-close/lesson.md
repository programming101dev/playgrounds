# Close of an unknown descriptor

This case tries to close an unopened positive descriptor.

Expected result: `p101 check` exits `1` and the correlated report includes
`P101-FD-003`.

Closing a descriptor you did not acquire is a privilege and correctness smell.
The number might be meaningless, or worse, it might name a real resource owned
by a different part of the program.

## What to look for

- Cleanup that closes a descriptor variable without proving acquisition.
- Sentinel values other than `-1`.
- Descriptor variables that are not initialized before cleanup.

## Fix path

1. Initialize descriptor variables to `-1`.
2. Only assign them from successful acquisition calls.
3. Only close descriptors that are not `-1`.
4. Clear them back to `-1` after close.

Re-run `./lab.sh`. This lab is fixed when `P101-FD-003` disappears for
`stray-close`.
