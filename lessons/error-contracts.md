# Make environment and error ownership explicit

Every fallible p101 call needs a visible environment and an error object whose
owner is clear. Check an error before making another fallible call, because the
second operation can obscure the first failure. Passing `NULL` is reserved for
an explicitly documented best-effort boundary.

Create application and subsystem error/environment objects separately when
their lifetimes or policies differ. Destroy each object in the file or owner
that created it unless ownership is deliberately transferred.

Process termination is also an ownership decision. A helper reports failure
through its `p101_error` or return value; it does not call `exit`, `_Exit`,
`abort`, or an equivalent terminating API. This lets callers finish cleanup
and decide how to present the failure. Only `main` selects the application's
final process status.

Within each function, make cleanup and status selection converge on one final
return. A `void` function can simply fall through its closing brace. Avoid
branch-local early returns: record the outcome, run the shared cleanup path,
and return once.

Verify with:

```sh
p101 doctor -s src -s include
```

The checker follows source-level contracts; it cannot prove that every runtime
path handles an error correctly. Pair it with fault injection.
