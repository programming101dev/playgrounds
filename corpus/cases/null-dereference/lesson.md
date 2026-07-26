# Check NULL before dereferencing

The broken fixture records `pointer=null` with `action=dereference`. In real
code that is at best a crash and at worst a reachable denial-of-service bug.

The fixed version should branch on NULL before dereferencing.

Canonical anchors: CWE-476, CERT EXP34-C.
