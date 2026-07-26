# Realloc result remains owned

This case allocates a buffer, grows it with `p101_realloc`, and then forgets to
free the grown allocation.

`realloc` can make ownership feel blurry because the pointer value may change.
The rule is still simple: if `p101_realloc` succeeds, the returned pointer is
the owned allocation. That final pointer must eventually reach `p101_free`.

## What to look for

- A successful `p101_realloc`.
- The old pointer value being replaced by the grown pointer.
- No `p101_free` for the final pointer value.

## Fix path

1. Keep the final pointer value in an owned variable.
2. Route success and failure paths through cleanup.
3. Call `p101_free(env, buffer)` on the final pointer.
4. If using a temporary for safer realloc style, assign it back only after
   success.

Re-run `./lab.sh`. This lab is fixed when `P101-ALLOC-001` disappears for
`realloc-leak`.
