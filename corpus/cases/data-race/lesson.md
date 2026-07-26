# Synchronize shared mutable state

The broken fixture records two threads updating shared state with
`synchronization=none`. That is the shape of a data race.

The fixed version should protect shared mutable state with a mutex or a suitable
atomic operation.

Canonical anchors: CWE-362, CERT CON rules.
