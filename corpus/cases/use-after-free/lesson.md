# Do not use ownership after free

The broken fixture records a pointer whose state is `freed` and then continues
with `action=used`. Real code that follows this shape can become a crash, data
corruption, or an exploitable use-after-free.

The fixed version should keep ownership live until the final use, or clear the
pointer and reject later use.

Canonical anchors: CWE-416, CERT MEM30-C.
