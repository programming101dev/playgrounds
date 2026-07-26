# Preserve the original pointer when realloc fails

The broken fixture records `original_pointer=lost`. That is the classic realloc
mistake: the program overwrites the only pointer before it knows whether the grow
succeeded.

The fixed version should use a temporary pointer and preserve the original
allocation on failure.

Canonical anchors: CWE-401, CERT MEM31-C, CERT MEM35-C.
