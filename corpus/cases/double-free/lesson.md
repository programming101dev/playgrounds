# Double free

This case allocates one heap block and records two releases of the same pointer.
The playground emits the first bad lifecycle event explicitly and performs only
one real `p101_free`, so the teaching process cannot abort before the analyzer
receives its completion record. The event sequence is the same ownership error
that two real frees would create.

Modern sanitizers usually catch this immediately, but the underlying lesson is
ownership. If two paths both believe they own the same pointer, cleanup becomes
dangerous. In real programs this shape can become memory corruption.

## What to look for

- Two release paths that can receive the same pointer value.
- A pointer that is not reset to `NULL` after release.
- Two cleanup branches that both think they own the allocation.

## Fix path

1. Choose one owner for the allocation.
2. Release it exactly once.
3. Remove the extra release event and set the pointer to `NULL` after
   `p101_free`.
4. Guard cleanup with `if(buffer != NULL)` when that makes ownership clearer.

Re-run `./lab.sh`. This lab is fixed when `P101-ALLOC-002` disappears for
`double-free`.
