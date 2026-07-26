# Reject writes past the end of a buffer

The broken fixture accepts `index=16` for `capacity=16`. C arrays are zero-based,
so that index names one element past the buffer.

The fixed version should reject writes unless `index < capacity`.

Canonical anchors: CWE-787, CERT ARR30-C, CERT STR31-C.
