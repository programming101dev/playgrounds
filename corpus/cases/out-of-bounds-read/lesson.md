# Reject reads past valid data

The broken fixture accepts a read at `index=16` when only 16 bytes are valid.
That is one past the end.

The fixed version should test against the valid data length, not just whether a
buffer exists.

Canonical anchors: CWE-125, CERT ARR30-C.
