# Error-path leak

This case is clean without injection but intentionally mishandles cleanup after
some injected p101 setup failures.

Expected result: `p101 check` exits `1`; the ordinary observed run can be clean,
but the fault-walk artifacts show resource findings in error paths.

This is the security lesson hiding inside the whole toolchain. Many real bugs
live on paths that “never happen” during normal tests: allocation failure,
short writes, failed opens, failed pipes, interrupted setup. Fault injection
makes those paths happen.

## What to look for

- The ordinary observed run is clean.
- Fault-walk runs fail one p101 call at a time.
- A resource acquired before the injected failure is not released afterward.

## Fix path

1. Open the linked fault-walk directory for this lab.
2. Find the first injected call that produces a finding.
3. Move cleanup out of any success-only branch.
4. Make cleanup conditional on ownership markers, not on “no error happened.”

Re-run `./lab.sh`. This lab is fixed when the fault-walk no longer finds leaked
resources for `fault-lab`.
