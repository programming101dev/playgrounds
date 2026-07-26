# Ignoring read count writes stale bytes

This case reads a short message into a 128-byte buffer and then writes the whole
buffer to the output file.

That is a classic information-disclosure bug. The buffer capacity is 128 bytes,
but only the bytes returned by `p101_read` are valid message data. Everything
else is stale buffer contents.

## What to look for

- A `p101_read` return value that is saved but not used for the next write.
- `sizeof(buffer)` used as though it meant “valid bytes.”
- Output larger than the input message.

## Fix path

1. Keep the `ssize_t` returned by `p101_read`.
2. Check it for failure.
3. Pass that value to `p101_write`.
4. Re-run `./lab.sh`.

This lab is fixed when `P101-C-002` moves from `OPEN` to `FIXED`.
