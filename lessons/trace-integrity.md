# Keep call traces balanced and trustworthy

These examples cover unbalanced or mismatched observed call traces.
Each section gives a broken input, its expected diagnostic, a repaired input, and the expected clean result.
Canonical correct programs remain in the corresponding example repository; native detection evidence remains in the owning tool suite.

<a id="P101-TRACE-001"></a>

## P101-TRACE-001 — call exit has no active entry

Broken input:

```text
CALL_EXIT operation
```

Expected diagnostic:

```text
P101-TRACE-001: call exit has no active entry
```

Repaired input:

```text
CALL_ENTER operation
CALL_EXIT operation
```

Expected clean result:

```text
No P101-TRACE-001 finding is emitted for the repaired input.
```

<a id="P101-TRACE-002"></a>

## P101-TRACE-002 — call exit mismatches the active call

Broken input:

```text
CALL_ENTER first
CALL_EXIT second
```

Expected diagnostic:

```text
P101-TRACE-002: call exit mismatches the active call
```

Repaired input:

```text
CALL_ENTER first
CALL_EXIT first
```

Expected clean result:

```text
No P101-TRACE-002 finding is emitted for the repaired input.
```

<a id="P101-TRACE-003"></a>

## P101-TRACE-003 — call remains open at end of stream

Broken input:

```text
CALL_ENTER operation
END
```

Expected diagnostic:

```text
P101-TRACE-003: call remains open at end of stream
```

Repaired input:

```text
CALL_ENTER operation
CALL_EXIT operation
END
```

Expected clean result:

```text
No P101-TRACE-003 finding is emitted for the repaired input.
```

## Platform boundary

Trace integrity covers calls that emit p101 trace events; missing instrumentation cannot be balanced by the analyzer.

## Correct reference

See [an environment configured with a tracer](https://github.com/programming101dev/lib_env_examples/blob/main/env/get_tracer/main.c).

## Verification boundary

Run `p101-inspect run -- <command>`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
