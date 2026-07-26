# Error-path leak

This case is clean without injection but intentionally mishandles cleanup after
some injected p101 setup failures.

Expected result: `p101 check` exits `1`; the ordinary observed run can be clean,
but the fault-walk artifacts show resource findings in error paths.
