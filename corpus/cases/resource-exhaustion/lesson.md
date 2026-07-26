# Put limits on resource requests

The broken fixture accepts an unbounded allocation request. Good C programs put
small, documented limits near the boundary of the program.

The fixed version should enforce a maximum before acquiring resources.

Canonical anchors: CWE-770, CERT MEM11-C.
