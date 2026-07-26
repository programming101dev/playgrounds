# Clean p101 resource handling

This case runs the playground `tour` scenario. It opens files, allocates and
reallocates memory, uses a pipe, forks a child, and cleans everything up.

Expected result: `p101 check` exits `0` and reports no resource findings.
