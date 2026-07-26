# Free of unknown pointer

This case allocates a buffer but passes an interior pointer to `p101_free`.

That is invalid. The allocator can only release the exact pointer it returned.
Freeing stack addresses, string literals, interior pointers, or borrowed
pointers is a classic C memory-corruption trap.

## What to look for

- Pointer arithmetic before `p101_free`.
- A pointer whose ownership was borrowed from somewhere else.
- Cleanup code that does not preserve the original owning pointer.

## Fix path

1. Keep the original allocation pointer.
2. Use separate variables for interior cursors or borrowed views.
3. Call `p101_free` only on the original owned pointer.
4. Set the owner pointer to `NULL` after release.

Re-run `./lab.sh`. This lab is fixed when `P101-ALLOC-003` disappears for
`stray-free`.
