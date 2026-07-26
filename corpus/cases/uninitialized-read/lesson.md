# Initialize values before reading them

The broken fixture records `initialized=false` and still uses the value. In real
C, reading an uninitialized automatic object is undefined or indeterminate,
depending on the type and context.

The fixed version should initialize state before using it.

Canonical anchors: CWE-457, CERT EXP33-C.
