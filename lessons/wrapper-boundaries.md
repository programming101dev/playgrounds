# Keep wrapper boundaries complete and observable

A wrapper is part of the instrumentation boundary. Calling the native function
directly can make fault injection, tracing, and resource tracking silently miss
student code. A wrapper with the wrong parameter order, target mapping, trace,
fault hook, or lifecycle hook is just as dangerous because it looks covered
while emitting incomplete evidence.

Use the audit evidence to identify the exact call or wrapper contract that
failed. Repair the smallest boundary: call the available wrapper, or make the
wrapper preserve the native contract while accepting `env` and `err` in the
standard positions. Do not suppress an external call until you have decided
whether it is an intentional dependency boundary.

Verify with:

```sh
p101 audit --strict-external src include
```

The audit is bounded by the compilation database and Clang translation units it
can parse. A clean result does not prove that third-party code is instrumented.
