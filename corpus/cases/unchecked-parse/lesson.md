# Reject trailing junk after numeric parses

The broken fixture accepts `123abc` because the numeric prefix looked valid.
That is how command-line flags and config values quietly turn into surprises.

The fixed version should validate the full string, detect range errors, and
reject trailing junk.

Canonical anchors: CERT INT/ERR conversion guidance, CWE-20.
