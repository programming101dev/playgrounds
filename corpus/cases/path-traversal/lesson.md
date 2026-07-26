# Keep paths inside their intended root

The broken fixture accepts `/safe/../secret.txt`. Text-prefix checks are not
enough; paths need normalization and root checks that cannot be fooled by `..`.

The fixed version should reject escaped paths.

Canonical anchors: CWE-22, CWE-20.
