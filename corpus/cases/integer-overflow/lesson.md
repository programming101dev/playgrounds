# Check size calculations before allocation

The broken fixture records a size multiplication that wrapped. The dangerous
version of this bug allocates a small buffer and then writes the larger intended
amount.

The fixed version should check multiplication before allocation.

Canonical anchors: CWE-190, CWE-680, CERT INT30-C, CERT INT32-C.
