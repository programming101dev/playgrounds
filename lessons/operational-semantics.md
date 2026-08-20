# Preserve progress and process obligations

These diagnostics cover semantic obligations that ordinary compiler warnings cannot establish from types alone.

<a id="P101-IO-001"></a>

## P101-IO-001 — a short or partial result is ignored

Broken input:

```text
p101_write(env, err, fd, data, size);
```

Expected diagnostic:

```text
P101-IO-001: a short or partial result was discarded
```

Repaired input:

```text
written = p101_write(env, err, fd, data, size);
check the error, then advance by exactly written bytes
```

Expected clean result:

```text
No P101-IO-001 finding is emitted when the annotated partial-result value is retained.
```

<a id="P101-RETRY-001"></a>

## P101-RETRY-001 — uncertain progress is retried without resolution

Broken input:

```text
operation_with_uncertain_progress();
operation_with_uncertain_progress();
```

Expected diagnostic:

```text
P101-RETRY-001: an operation with uncertain progress was repeated without an intervening resolution
```

Repaired input:

```text
status = operation_with_uncertain_progress();
resolve whether progress committed before deciding whether a retry is safe
```

Expected clean result:

```text
No P101-RETRY-001 finding is emitted when the operation is not blindly repeated.
```

<a id="P101-SYNC-006"></a>

## P101-SYNC-006 — a condition wait is not guarded by a predicate loop

Broken input:

```text
if(!ready) { p101_pthread_cond_wait(env, err, &condition, &mutex); }
```

Expected diagnostic:

```text
P101-SYNC-006: a condition wait is not lexically guarded by a predicate loop
```

Repaired input:

```text
while(!ready) { status = p101_pthread_cond_wait(env, err, &condition, &mutex); check(status, err); }
```

Expected clean result:

```text
No P101-SYNC-006 finding is emitted when every wakeup rechecks the predicate.
```

The checker recognizes the wait by its semantic role and the loop through the Clang AST. It does not depend on function or variable spelling.

<a id="P101-PROC-002"></a>

## P101-PROC-002 — unsafe work occurs after fork and before exec

Broken input:

```text
post-fork child function calls an operation without the p101:process:post-fork-safe role
```

Expected diagnostic:

```text
P101-PROC-002: a post-fork child path calls an operation not declared async-signal-safe before exec
```

Repaired input:

```text
restrict the annotated child path to declared safe operations and exec immediately
```

Expected clean result:

```text
No P101-PROC-002 finding is emitted for the annotated safe child path.
```

## Platform boundary

Static rules require explicit semantic roles; runtime process obligations require instrumented wrapper events. Direct native calls and unexecuted paths remain blind spots.

## Correct reference

See [a child process that is explicitly reaped](https://github.com/programming101dev/c-examples/blob/main/process/wait/main.c).

## Verification boundary

Run the error-contract and runtime-analysis suites named by the shared lesson manifest. A clean result is bounded by admitted Clang facts and observed lifecycle events.
