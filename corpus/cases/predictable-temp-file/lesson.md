# Avoid predictable temporary files

`/tmp/name.tmp` looks convenient, but another process can guess it. Depending on
how the real program opens the path, that can become a clobber, symlink, or
information-disclosure bug.

The fix is an exclusive-create pattern such as `mkstemp`/`open` with appropriate
flags, not a timestamp or PID glued onto a filename.

Canonical anchors: CWE-377, CERT FIO21-C.
