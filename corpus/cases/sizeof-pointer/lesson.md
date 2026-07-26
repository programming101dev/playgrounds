# `sizeof(pointer)` is not buffer length

This case allocates `args->bytes` bytes but writes `sizeof(buffer)` bytes.
Because `buffer` is a pointer, `sizeof(buffer)` is the size of the pointer, not
the allocation.

On this platform the broken fixture writes 8 bytes. The intended output is the
default 64-byte buffer.

## What to look for

- `sizeof(name)` where `name` is a pointer.
- A function that lost the original array length.
- Buffer code that does not carry an explicit length alongside the pointer.

## Fix path

1. Find the length that describes the allocation.
2. Pass that length explicitly.
3. Replace `sizeof(buffer)` with `args->bytes` in this lab.
4. Re-run `./lab.sh`.

This lab is fixed when `P101-C-001` moves from `OPEN` to `FIXED`.
