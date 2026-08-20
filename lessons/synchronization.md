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

<a id="P101-SYNC-004"></a>

## P101-SYNC-004 — locks are released out of acquisition order

Broken input:

```text
lock A; lock B; unlock A; unlock B
```

Expected diagnostic:

```text
P101-SYNC-004: locks were not released in reverse acquisition order
```

Repaired input:

```text
lock A; lock B; unlock B; unlock A
```

Expected clean result:

```text
No P101-SYNC-004 finding is emitted for the repaired input.
```

Reverse release is a p101 structured-locking policy rather than a universal POSIX requirement. It keeps cleanup predictable and makes the interval protected by every nested lock visible during review and debugging.

<a id="P101-SYNC-005"></a>

## P101-SYNC-005 — a thread join begins while a lock is held

Broken input:

```text
lock A; join worker B while B may need A to finish
```

Expected diagnostic:

```text
P101-SYNC-005: a thread join began while a lock was held
```

Repaired input:

```text
finish the protected update; unlock A; then join worker B
```

Expected clean result:

```text
No P101-SYNC-005 finding is emitted for the repaired input.
```

This rule uses explicit synchronization resource events. It deliberately does not reject nested lock acquisition or condition waits: ordered nesting can be valid, and a condition wait atomically releases its mutex. It does not guess from function names, and it cannot see blocking operations or callbacks that do not emit the shared event protocol.

<a id="P101-SYNC-007"></a>

## P101-SYNC-007 — mutex destruction overlaps an active lock or wait

Broken input:

```c
lock_status    = p101_pthread_mutex_lock(env, err, &mutex);
destroy_status = p101_pthread_mutex_destroy(env, err, &mutex);
```

Destruction does not cancel an owner or waiter. It ends the object's lifetime
while another operation still depends on it. The runtime checker relates the
mutex resource identity to active ownership and wait events.

Expected diagnostic:

```text
P101-SYNC-007: a mutex was destroyed while it was locked or had an active waiter
```

Repaired input:

```c
lock_status   = p101_pthread_mutex_lock(env, err, &mutex);
work_status   = update_shared_state();
unlock_status = p101_pthread_mutex_unlock(env, err, &mutex);
join_status   = p101_pthread_join(env, err, worker, NULL);
destroy_status = p101_pthread_mutex_destroy(env, err, &mutex);
```

Production code must also respond to each status. The important ordering is
that new users are stopped and every possible owner or waiter has completed
before destruction.

Expected clean result:

```text
No P101-SYNC-007 finding is emitted after every lock owner and waiter has finished.
```

<a id="P101-SYNC-008"></a>

## P101-SYNC-008 — thread termination is consumed more than once

Broken input:

```c
join_status   = p101_pthread_join(env, err, thread, NULL);
detach_status = p101_pthread_detach(env, err, thread);
```

Joining or detaching consumes the joinable lifetime. A second terminal
operation is not cleanup; it is an invalid use of an already-consumed thread
resource.

Expected diagnostic:

```text
P101-SYNC-008: a thread was joined or detached after its joinable lifetime had already ended
```

Repaired input:

```c
if(ownership == THREAD_OWNER_JOIN)
{
    terminal_status = p101_pthread_join(env, err, thread, NULL);
}
else
{
    terminal_status = p101_pthread_detach(env, err, thread);
}
```

Exactly one component owns the terminal operation. The checker uses the
thread-resource identity and terminal-attempt events, not the names of the
variables or functions containing them.

Expected clean result:

```text
No P101-SYNC-008 finding is emitted when one terminal operation consumes the thread lifetime.
```

<a id="P101-SYNC-009"></a>

## P101-SYNC-009 — one condition variable is waited on with different mutexes

Broken input:

```c
wait_a_status = p101_pthread_cond_wait(env, err, &condition, &mutex_a);
wait_b_status = p101_pthread_cond_wait(env, err, &condition, &mutex_b);
```

Concurrent waits on one condition variable must agree on the mutex protecting
its predicate. Different mutexes make the relationship between notification
and predicate observation incoherent.

Expected diagnostic:

```text
P101-SYNC-009: concurrent waits associate one condition variable with different mutexes
```

Repaired input:

```c
wait_a_status = p101_pthread_cond_wait(env, err, &condition, &predicate_mutex);
wait_b_status = p101_pthread_cond_wait(env, err, &condition, &predicate_mutex);
```

The event protocol records both the condition identity and related mutex
identity, allowing the checker to compare concurrent waits without relying on
source-level spelling.

Expected clean result:

```text
No P101-SYNC-009 finding is emitted when every concurrent wait uses the same mutex.
```

<a id="P101-SYNC-010"></a>

## P101-SYNC-010 — blocking operation begins while a lock is held

Broken input:

```c
lock_status = p101_pthread_mutex_lock(env, err, &mutex);
read_count  = p101_read(env, err, descriptor, buffer, buffer_size);
```

An unbounded wait while owning a lock can prevent every other participant from
making the progress needed to unblock it.

Expected diagnostic:

```text
P101-SYNC-010: a potentially blocking operation began while a lock was held
```

Repaired input:

```c
lock_status   = p101_pthread_mutex_lock(env, err, &mutex);
descriptor    = shared_descriptor;
unlock_status = p101_pthread_mutex_unlock(env, err, &mutex);
read_count    = p101_read(env, err, descriptor, buffer, buffer_size);
```

The checker recognizes blocking-operation and held-lock semantic events.
Condition waits are modeled separately because they atomically release their
associated mutex while blocked.

Expected clean result:

```text
No P101-SYNC-010 finding is emitted when the blocking operation begins outside the critical section.
```

Condition waits are excluded because their contract atomically releases the associated mutex while blocked. This rule is intentionally opinionated: a documented design can be structurally refactored, but silent blocking inside a critical section is rejected.

## Platform boundary

The graph contains only synchronization events observed during the run; a clean run does not prove that an unexecuted schedule is cycle-free.

## Correct reference

See [one mutex protecting one critical section](https://github.com/programming101dev/c-examples/blob/main/pthread-mutex/pthread_mutex_lock-pthread_mutex_unlock/main.c).

## Verification boundary

Run `scripts/runtime/student-workflow.sh -- <multithreaded-command>`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
