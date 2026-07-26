# Clear secret data before reusing storage

The broken fixture records `old_secret=api-key-123` leaking from reused storage.
This is a teaching version of stale-data exposure.

The fixed version should clear sensitive bytes before reuse or release.

Canonical anchors: CWE-226, CWE-200, CERT MEM03-C, CERT MEM06-C.
