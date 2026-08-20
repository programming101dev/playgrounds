# Turn observed behavior into an executable expectation

These examples cover declared behavioral expectations that observed evidence violates.
Each section gives a broken input, its expected diagnostic, a repaired input, and the expected clean result.
Canonical correct programs remain in the corresponding example repository; native detection evidence remains in the owning tool suite.

<a id="P101-EXPECT-001"></a>

## P101-EXPECT-001 — observed model violates an expectation

Broken input:

```text
expect no P101-FD-*; observed P101-FD-001
```

Expected diagnostic:

```text
P101-EXPECT-001: observed model violates an expectation
```

Repaired input:

```text
expect no P101-FD-*; observed report contains no P101-FD-* finding
```

Expected clean result:

```text
No P101-EXPECT-001 finding is emitted for the repaired input.
```

<a id="P101-POLICY-RESOURCE-001"></a>

## P101-POLICY-RESOURCE-001 — descriptor policy forbids an observed finding

Broken input:

```text
policy forbids P101-FD-*; report contains P101-FD-001
```

Expected diagnostic:

```text
P101-POLICY-RESOURCE-001: descriptor policy forbids an observed finding
```

Repaired input:

```text
policy forbids P101-FD-*; report contains no P101-FD-* finding
```

Expected clean result:

```text
No P101-POLICY-RESOURCE-001 finding is emitted for the repaired input.
```

<a id="P101-POLICY-RESOURCE-002"></a>

## P101-POLICY-RESOURCE-002 — allocation policy forbids an observed finding

Broken input:

```text
policy forbids P101-ALLOC-*; report contains P101-ALLOC-001
```

Expected diagnostic:

```text
P101-POLICY-RESOURCE-002: allocation policy forbids an observed finding
```

Repaired input:

```text
policy forbids P101-ALLOC-*; report contains no P101-ALLOC-* finding
```

Expected clean result:

```text
No P101-POLICY-RESOURCE-002 finding is emitted for the repaired input.
```

<a id="P101-POLICY-RESOURCE-003"></a>

## P101-POLICY-RESOURCE-003 — generic-resource policy forbids an observed finding

Broken input:

```text
policy forbids P101-RESOURCE-*; report contains P101-RESOURCE-001
```

Expected diagnostic:

```text
P101-POLICY-RESOURCE-003: generic-resource policy forbids an observed finding
```

Repaired input:

```text
policy forbids P101-RESOURCE-*; report contains no P101-RESOURCE-* finding
```

Expected clean result:

```text
No P101-POLICY-RESOURCE-003 finding is emitted for the repaired input.
```

<a id="P101-POLICY-SYNC-001"></a>

## P101-POLICY-SYNC-001 — synchronization behavior is not clean

Broken input:

```text
concurrency forbids P101-SYNC-*; report contains P101-SYNC-001
```

Expected diagnostic:

```text
P101-POLICY-SYNC-001: synchronization behavior is not clean
```

Repaired input:

```text
concurrency forbids P101-SYNC-*; report contains no P101-SYNC-* finding
```

Expected clean result:

```text
No P101-POLICY-SYNC-001 finding is emitted for the repaired input.
```

<a id="P101-POLICY-SECURE-001"></a>

## P101-POLICY-SECURE-001 — descriptor misuse can cross a trust boundary

Broken input:

```text
secure-c forbids P101-FD-*; report contains P101-FD-004
```

Expected diagnostic:

```text
P101-POLICY-SECURE-001: descriptor misuse can cross a trust boundary
```

Repaired input:

```text
secure-c forbids P101-FD-*; report contains no P101-FD-* finding
```

Expected clean result:

```text
No P101-POLICY-SECURE-001 finding is emitted for the repaired input.
```

<a id="P101-POLICY-SECURE-002"></a>

## P101-POLICY-SECURE-002 — allocation misuse can corrupt memory

Broken input:

```text
secure-c forbids P101-ALLOC-*; report contains P101-ALLOC-003
```

Expected diagnostic:

```text
P101-POLICY-SECURE-002: allocation misuse can corrupt memory
```

Repaired input:

```text
secure-c forbids P101-ALLOC-*; report contains no P101-ALLOC-* finding
```

Expected clean result:

```text
No P101-POLICY-SECURE-002 finding is emitted for the repaired input.
```

<a id="P101-POLICY-SECURE-003"></a>

## P101-POLICY-SECURE-003 — resource ownership is ambiguous

Broken input:

```text
secure-c forbids P101-RESOURCE-*; report contains P101-RESOURCE-002
```

Expected diagnostic:

```text
P101-POLICY-SECURE-003: resource ownership is ambiguous
```

Repaired input:

```text
secure-c forbids P101-RESOURCE-*; report contains no P101-RESOURCE-* finding
```

Expected clean result:

```text
No P101-POLICY-SECURE-003 finding is emitted for the repaired input.
```

<a id="P101-POLICY-SECURE-004"></a>

## P101-POLICY-SECURE-004 — observed synchronization is unsafe

Broken input:

```text
secure-c forbids P101-SYNC-*; report contains P101-SYNC-002
```

Expected diagnostic:

```text
P101-POLICY-SECURE-004: observed synchronization is unsafe
```

Repaired input:

```text
secure-c forbids P101-SYNC-*; report contains no P101-SYNC-* finding
```

Expected clean result:

```text
No P101-POLICY-SECURE-004 finding is emitted for the repaired input.
```

## Platform boundary

Policy evaluates findings present in the admitted analysis directory; it cannot forbid behavior that the capture and analysis stages did not observe.

## Correct references

- [Matched allocation ownership](https://github.com/programming101dev/c-examples/blob/main/memory/malloc-free/main.c).
- [One mutex protecting one critical section](https://github.com/programming101dev/c-examples/blob/main/pthread-mutex/pthread_mutex_lock-pthread_mutex_unlock/main.c).

## Verification boundary

Run `p101-inspect model check <analysis-directory> --rules <rule-pack>`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
