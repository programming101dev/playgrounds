# Keep call traces balanced and trustworthy

An exit without a matching entry, an entry left open, or a mismatched nesting
order means the call tree cannot be trusted. This often comes from missing
scope cleanup, abrupt control flow, or sharing a tracing context incorrectly.

Use one trace scope for every instrumented function and make all exits pass
through its cleanup boundary. Do not repair the renderer; repair the event
producer or the code path that bypassed it.

Verify with:

```sh
p101-inspect run -- ./your-program
```

The trace includes p101-aware functions only. It is deliberately not a complete
replacement for an operating-system tracer.
