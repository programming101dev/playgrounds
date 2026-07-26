# Do not follow untrusted symlinks for sensitive opens

The broken fixture follows `/tmp/p101-link`. If an attacker controls that link,
the program may read or overwrite the wrong file.

The fixed version should use a safe open pattern and verify the opened object,
not just the path string.

Canonical anchors: CWE-59, CWE-61, CERT FIO guidance.
