# Do not treat unterminated bytes as a C string

The broken fixture copies 16 bytes into 16 bytes of capacity and then treats the
result as a C string. There is no room for the terminating NUL byte.

The fixed version should either keep the value as a counted byte buffer or
reserve space for a terminator and write it.

Canonical anchors: CERT STR rules, CWE-170.
