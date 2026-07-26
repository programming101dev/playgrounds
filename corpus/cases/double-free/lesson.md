# Double free

This case allocates one heap block and releases the same pointer twice.

Modern sanitizers usually catch this immediately, but the underlying lesson is
ownership. If two paths both believe they own the same pointer, cleanup becomes
dangerous. In real programs this shape can become memory corruption.

## What to look for

- Two `p101_free` calls that can receive the same pointer value.
- A pointer that is not reset to `NULL` after release.
- Two cleanup branches that both think they own the allocation.

## Fix path

1. Choose one owner for the allocation.
2. Release it exactly once.
3. Set the pointer to `NULL` after `p101_free`.
4. Guard cleanup with `if(buffer != NULL)` when that makes ownership clearer.

Re-run `./lab.sh`. This lab is fixed when `P101-ALLOC-002` disappears for
`double-free`.
