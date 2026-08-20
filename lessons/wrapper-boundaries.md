# Keep wrapper boundaries complete and observable

A wrapper is part of the instrumentation boundary. Calling the native function
directly can make fault injection, tracing, and resource tracking silently miss
student code.

Use the audit evidence to identify the exact call that crossed the boundary.
Repair the smallest boundary by calling the available wrapper. Do not suppress
an external call until you have decided whether it is an intentional dependency
boundary.

Wrapper implementations themselves have a separate executable contract:
generated unit tests exercise normal return, failure, tracing entry/exit, and
resource events. When writing a wrapper library, run that library's `cmake -S . -B build -DP101_BUILD_LEVEL=2 && cmake --build build`
as well as this boundary audit; a call-site audit cannot prove the wrapper's
runtime behavior.

Verify with:

```sh
../programs/p101-audit/audit-wrappers --strict-external src include
```

The audit is bounded by the compilation database and Clang translation units it
can parse. A clean result does not prove that third-party code is instrumented.

For a small correct wrapper call, see the
[lib_c `abs` example](https://github.com/programming101dev/lib_c_examples/blob/main/stdlib/abs/main.c).
The example repository owns the good path; this lesson owns the missed-wrapper
defect and its repair.
