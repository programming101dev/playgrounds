# Descriptor leak

This case intentionally leaves an output descriptor open.

Expected result: `p101 check` exits `1` and the correlated report includes
`P101-FD-001`.
