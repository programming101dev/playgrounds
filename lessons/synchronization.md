# Reason about lock, wait, and join graphs

These examples cover cycles in observed lock, wait, and join relationships.
Each section gives a broken input, its expected diagnostic, a repaired input, and the expected clean result.
Canonical correct programs remain in the corresponding example repository; native detection evidence remains in the owning tool suite.

<a id="P101-SYNC-001"></a>

## P101-SYNC-001 — observed lock order contains a cycle

Broken input:

```text
thread A: lock one then two
thread B: lock two then one
```

Expected diagnostic:

```text
P101-SYNC-001: observed lock order contains a cycle
```

Repaired input:

```text
thread A: lock one then two
thread B: lock one then two
```

Expected clean result:

```text
No P101-SYNC-001 finding is emitted for the repaired input.
```

<a id="P101-SYNC-002"></a>

## P101-SYNC-002 — observed wait graph contains a cycle

Broken input:

```text
thread A waits for B; thread B waits for A
```

Expected diagnostic:

```text
P101-SYNC-002: observed wait graph contains a cycle
```

Repaired input:

```text
thread A waits for B; thread B completes and signals A
```

Expected clean result:

```text
No P101-SYNC-002 finding is emitted for the repaired input.
```

<a id="P101-SYNC-003"></a>

## P101-SYNC-003 — observed join graph contains a cycle

Broken input:

```text
thread A joins B; thread B joins A
```

Expected diagnostic:

```text
P101-SYNC-003: observed join graph contains a cycle
```

Repaired input:

```text
owner joins A; owner joins B
```

Expected clean result:

```text
No P101-SYNC-003 finding is emitted for the repaired input.
```

## Platform boundary

The graph contains only synchronization events observed during the run; a clean run does not prove that an unexecuted schedule is cycle-free.

## Correct reference

See [one mutex protecting one critical section](https://github.com/programming101dev/c-examples/blob/main/pthread-mutex/pthread_mutex_lock-pthread_mutex_unlock/main.c).

## Verification boundary

Run `scripts/runtime/student-workflow.sh -- <multithreaded-command>`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
