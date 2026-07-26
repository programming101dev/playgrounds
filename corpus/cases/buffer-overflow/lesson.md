# Do not copy more bytes than the destination holds

The broken fixture accepts a 32-byte copy into a 16-byte destination. That is the
shape behind many stack and heap buffer overflows.

The fixed version should compare source length and destination capacity before
copying.

Canonical anchors: CWE-120, CWE-121, CWE-122.
