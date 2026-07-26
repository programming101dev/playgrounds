# Partial cleanup after multiple acquisitions

This case acquires a file descriptor, a heap buffer, and a pipe. It closes one
pipe end and forgets the rest.

This is where C cleanup starts to feel slippery. The function has more than one
owned resource, and each resource has a different invalid value and release
function. Guessing at cleanup is how real bugs survive.

## What to look for

- More than one successful acquisition in the same function.
- Cleanup that handles only the last resource touched.
- Missing invalid markers such as `-1` for descriptors and `NULL` for pointers.

## Fix path

1. Treat cleanup as a ledger: one variable per resource.
2. Initialize all descriptors to `-1` and all pointers to `NULL`.
3. Use one cleanup block that checks and releases every resource.
4. Clear descriptors after close.

Re-run `./lab.sh`. This lab is fixed when the descriptor and allocation findings
disappear for `partial-cleanup`.
