# Allocation leak

This case intentionally leaves one heap allocation live at process exit.

Expected result: `p101 check` exits `1` and the correlated report includes
`P101-ALLOC-001`.
