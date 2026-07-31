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
resource events. When writing a wrapper library, run that library's `./test.sh`
as well as this boundary audit; a call-site audit cannot prove the wrapper's
runtime behavior.

Verify with:

```sh
p101 audit --strict-external src include
```

The audit is bounded by the compilation database and Clang translation units it
can parse. A clean result does not prove that third-party code is instrumented.
