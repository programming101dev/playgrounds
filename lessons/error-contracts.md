# Make environment and error ownership explicit

Every fallible p101 call needs a visible environment and an error object whose
owner is clear. Check an error before making another fallible call, because the
second operation can obscure the first failure. Passing `NULL` is reserved for
an explicitly documented best-effort boundary.

Create application and subsystem error/environment objects separately when
their lifetimes or policies differ. Destroy each object in the file or owner
that created it unless ownership is deliberately transferred.

Verify with:

```sh
p101 doctor -s src -s include
```

The checker follows source-level contracts; it cannot prove that every runtime
path handles an error correctly. Pair it with fault injection.
