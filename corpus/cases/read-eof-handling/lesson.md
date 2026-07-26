# Distinguish EOF from read errors

The broken fixture records `result=0` with `meaning=error`. For `read`-style
interfaces, negative means error, zero means EOF, and positive means the number
of bytes received.

The fixed version should have distinct branches for those three cases.

Canonical anchors: POSIX `read`, CERT FIO guidance.
