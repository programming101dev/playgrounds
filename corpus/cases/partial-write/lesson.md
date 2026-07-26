# Handle short writes

The broken fixture accepts `written=17` for a `requested=64` write. That can
happen on pipes, sockets, terminals, nonblocking descriptors, and interrupted
I/O.

The fixed version should loop until all bytes are written, or report a real
error.

Canonical anchors: CWE-252, CERT FIO write-count guidance.
