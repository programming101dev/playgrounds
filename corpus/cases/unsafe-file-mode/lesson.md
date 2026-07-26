# Create sensitive files with restrictive permissions

The broken fixture records `mode=0666` for a file containing a secret. The
process umask may reduce that mode, but secure code should not rely on a broad
request becoming safe by accident.

The fixed version should request restrictive permissions at creation time.

Canonical anchors: CWE-732, CERT FIO guidance.
