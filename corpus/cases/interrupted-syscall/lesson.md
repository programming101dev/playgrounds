# Retry interrupted system calls deliberately

The broken fixture treats `errno=EINTR` as a final failure. In systems programs,
signals can interrupt blocking calls such as `read`, `write`, `accept`, and
`waitpid`.

The fixed version should retry only when retrying is correct for that operation,
and it should preserve any partial progress already made.

Canonical anchors: POSIX signal/EINTR behavior, CERT ERR/FIO guidance.
