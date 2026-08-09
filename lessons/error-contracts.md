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

Keep every call visible as a separate operation. A call whose result is
intentionally ignored may stand alone. When another operation consumes a
result, the call must initialize or assign a named local before that value is
tested, passed, returned, cast, or combined:

```c
ready = is_ready();
if(ready)
{
    value = read_value();
    result = transform(value);
}
return result;
```

Do not hide work inside `if(is_ready())`, `transform(read_value())`,
`return transform(value)`, or `(void)transform(value)`. With a named local, a
debugger can stop after each operation, a log can show the intermediate value,
and the source makes evaluation order explicit. The compiler may optimize the
local away, so this is an observability rule rather than a promise of faster
machine code.

Verify with:

```sh
../programs/p101-audit/audit-doctor -s src -s include
```

The checker follows source-level contracts; it cannot prove that every runtime
path handles an error correctly. Pair it with fault injection.
