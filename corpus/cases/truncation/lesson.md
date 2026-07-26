# Do not silently truncate large values

The broken fixture stores `70000` in a 16-bit unsigned value. The result is not
`70000`; on the target machines it becomes `4464`.

The fixed version should check the range before narrowing and reject values that
cannot be represented exactly.

Canonical anchors: CWE-197, CERT INT rules.
