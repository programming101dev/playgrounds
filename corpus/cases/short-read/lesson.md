# Handle short reads

The broken fixture accepts `read=17` for a `requested=64` object. A successful
read can return less than requested; it only says how many bytes arrived this
time.

The fixed version should loop until the protocol or file object is complete,
EOF is reached, or a real error occurs.

Canonical anchors: POSIX `read`, CWE-252, CERT FIO guidance.
