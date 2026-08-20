# Model ownership for every resource class

Descriptors and heap blocks are only two resource classes. Directory streams,
dynamic-library handles, mappings, IPC objects, and synchronization objects all
have the same core lifecycle: acquire, possibly replace or transfer, and
release exactly once.

Write down the owner immediately after acquisition. Preserve the old identity
until replacement succeeds. Do not acquire the same logical identity twice
without first resolving who owns the earlier instance.

Verify the repaired command with:

```sh
../scripts/runtime/student-workflow.sh -- ./your-program
```

Only resources whose wrappers emit lifecycle events are visible. Pair a clean
report with the wrapper completeness audit.

For a compact correct create/use/destroy shape, see the
[lib_error lifecycle example](https://github.com/programming101dev/lib_error_examples/blob/main/lifecycle/main.c).
The concrete resource type differs, but the ownership boundary is the same.
