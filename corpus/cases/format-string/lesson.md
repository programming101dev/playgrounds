# Never use user text as a printf format

The broken fixture records user-controlled text being used as the format string.
That gives percent sequences special meaning.

The fixed version should use a constant format and pass untrusted text as data,
for example `"%s"`.

Canonical anchors: CWE-134, CERT FIO30-C, CERT FIO47-C.
