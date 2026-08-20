# Model ownership for every resource class

These examples cover observed descriptor, allocation, and generic-resource lifecycles.
Each section gives a broken input, its expected diagnostic, a repaired input, and the expected clean result.
Canonical correct programs remain in the corresponding example repository; native detection evidence remains in the owning tool suite.

<a id="P101-ALLOC-001"></a>

## P101-ALLOC-001 — allocation remains live

Broken input:

```text
alloc pointer has ACQUIRE but no RELEASE
```

Expected diagnostic:

```text
P101-ALLOC-001: allocation remains live
```

Repaired input:

```text
ALLOC_ACQUIRE pointer
ALLOC_RELEASE pointer
```

Expected clean result:

```text
No P101-ALLOC-001 finding is emitted for the repaired input.
```

<a id="P101-ALLOC-002"></a>

## P101-ALLOC-002 — allocation is freed twice

Broken input:

```text
ALLOC_RELEASE pointer
ALLOC_RELEASE pointer
```

Expected diagnostic:

```text
P101-ALLOC-002: allocation is freed twice
```

Repaired input:

```text
ALLOC_ACQUIRE pointer
ALLOC_RELEASE pointer
```

Expected clean result:

```text
No P101-ALLOC-002 finding is emitted for the repaired input.
```

<a id="P101-ALLOC-003"></a>

## P101-ALLOC-003 — unknown pointer is freed

Broken input:

```text
ALLOC_RELEASE pointer without observed ACQUIRE
```

Expected diagnostic:

```text
P101-ALLOC-003: unknown pointer is freed
```

Repaired input:

```text
ALLOC_ACQUIRE pointer
ALLOC_RELEASE pointer
```

Expected clean result:

```text
No P101-ALLOC-003 finding is emitted for the repaired input.
```

<a id="P101-ALLOC-004"></a>

## P101-ALLOC-004 — reallocation references a non-live pointer

Broken input:

```text
ALLOC_REPLACE old-pointer after old-pointer was released
```

Expected diagnostic:

```text
P101-ALLOC-004: reallocation references a non-live pointer
```

Repaired input:

```text
ALLOC_ACQUIRE old-pointer
ALLOC_REPLACE old-pointer new-pointer
ALLOC_RELEASE new-pointer
```

Expected clean result:

```text
No P101-ALLOC-004 finding is emitted for the repaired input.
```

<a id="P101-FD-001"></a>

## P101-FD-001 — descriptor remains open

Broken input:

```text
FD_ACQUIRE 5 without FD_RELEASE 5
```

Expected diagnostic:

```text
P101-FD-001: descriptor remains open
```

Repaired input:

```text
FD_ACQUIRE 5
FD_RELEASE 5
```

Expected clean result:

```text
No P101-FD-001 finding is emitted for the repaired input.
```

<a id="P101-FD-002"></a>

## P101-FD-002 — descriptor is closed twice

Broken input:

```text
FD_RELEASE 5
FD_RELEASE 5
```

Expected diagnostic:

```text
P101-FD-002: descriptor is closed twice
```

Repaired input:

```text
FD_ACQUIRE 5
FD_RELEASE 5
```

Expected clean result:

```text
No P101-FD-002 finding is emitted for the repaired input.
```

<a id="P101-FD-003"></a>

## P101-FD-003 — unknown descriptor is closed

Broken input:

```text
FD_RELEASE 5 without observed FD_ACQUIRE 5
```

Expected diagnostic:

```text
P101-FD-003: unknown descriptor is closed
```

Repaired input:

```text
FD_ACQUIRE 5
FD_RELEASE 5
```

Expected clean result:

```text
No P101-FD-003 finding is emitted for the repaired input.
```

<a id="P101-FD-004"></a>

## P101-FD-004 — descriptor can cross exec

Broken input:

```text
FD_ACQUIRE 5 with CLOEXEC false before exec
```

Expected diagnostic:

```text
P101-FD-004: descriptor can cross exec
```

Repaired input:

```text
FD_ACQUIRE 5 with CLOEXEC true before exec
```

Expected clean result:

```text
No P101-FD-004 finding is emitted for the repaired input.
```

<a id="P101-RESOURCE-000"></a>

## P101-RESOURCE-000 — resource event has an unknown lifecycle kind

Broken input:

```text
RESOURCE_EVENT kind=unknown
```

Expected diagnostic:

```text
P101-RESOURCE-000: resource event has an unknown lifecycle kind
```

Repaired input:

```text
RESOURCE_ACQUIRE handle
RESOURCE_RELEASE handle
```

Expected clean result:

```text
No P101-RESOURCE-000 finding is emitted for the repaired input.
```

<a id="P101-RESOURCE-001"></a>

## P101-RESOURCE-001 — generic resource remains live

Broken input:

```text
RESOURCE_ACQUIRE handle without RESOURCE_RELEASE handle
```

Expected diagnostic:

```text
P101-RESOURCE-001: generic resource remains live
```

Repaired input:

```text
RESOURCE_ACQUIRE handle
RESOURCE_RELEASE handle
```

Expected clean result:

```text
No P101-RESOURCE-001 finding is emitted for the repaired input.
```

<a id="P101-RESOURCE-002"></a>

## P101-RESOURCE-002 — generic resource is released twice

Broken input:

```text
RESOURCE_RELEASE handle
RESOURCE_RELEASE handle
```

Expected diagnostic:

```text
P101-RESOURCE-002: generic resource is released twice
```

Repaired input:

```text
RESOURCE_ACQUIRE handle
RESOURCE_RELEASE handle
```

Expected clean result:

```text
No P101-RESOURCE-002 finding is emitted for the repaired input.
```

<a id="P101-RESOURCE-003"></a>

## P101-RESOURCE-003 — unknown generic resource is released

Broken input:

```text
RESOURCE_RELEASE handle without observed acquisition
```

Expected diagnostic:

```text
P101-RESOURCE-003: unknown generic resource is released
```

Repaired input:

```text
RESOURCE_ACQUIRE handle
RESOURCE_RELEASE handle
```

Expected clean result:

```text
No P101-RESOURCE-003 finding is emitted for the repaired input.
```

<a id="P101-RESOURCE-004"></a>

## P101-RESOURCE-004 — resource replacement uses a non-live identity

Broken input:

```text
RESOURCE_REPLACE old after old was released
```

Expected diagnostic:

```text
P101-RESOURCE-004: resource replacement uses a non-live identity
```

Repaired input:

```text
RESOURCE_ACQUIRE old
RESOURCE_REPLACE old new
RESOURCE_RELEASE new
```

Expected clean result:

```text
No P101-RESOURCE-004 finding is emitted for the repaired input.
```

<a id="P101-RESOURCE-005"></a>

## P101-RESOURCE-005 — resource identity is acquired twice

Broken input:

```text
RESOURCE_ACQUIRE handle
RESOURCE_ACQUIRE handle
```

Expected diagnostic:

```text
P101-RESOURCE-005: resource identity is acquired twice
```

Repaired input:

```text
RESOURCE_ACQUIRE handle
RESOURCE_RELEASE handle
RESOURCE_ACQUIRE handle
```

Expected clean result:

```text
No P101-RESOURCE-005 finding is emitted for the repaired input.
```

<a id="P101-RESOURCE-007"></a>

## P101-RESOURCE-007 — a resource is used while it is not live

Broken input:

```text
RESOURCE_RELEASE handle
RESOURCE_USE handle
```

Expected diagnostic:

```text
P101-RESOURCE-007: resource was used before acquisition, after release, or after ownership transfer
```

Repaired input:

```text
RESOURCE_ACQUIRE handle
RESOURCE_USE handle
RESOURCE_RELEASE handle
```

Expected clean result:

```text
No P101-RESOURCE-007 finding is emitted for the repaired event sequence.
```

<a id="P101-PROC-001"></a>

## P101-PROC-001 — a child or joinable thread obligation remains live

Broken input:

```text
spawn child or create joinable thread; return without wait/join/detach
```

Expected diagnostic:

```text
P101-PROC-001: child process was not reaped or joinable thread was not joined or detached
```

Repaired input:

```text
wait for the child; join or detach the thread on every path
```

Expected clean result:

```text
No P101-PROC-001 finding is emitted when the observed lifecycle obligation is released.
```

## Platform boundary

Only resources observed through the instrumented wrappers are modeled; direct native calls and unobserved third-party ownership remain blind spots on every platform.

## Correct reference

See [explicit lifecycle ownership and one cleanup path](https://github.com/programming101dev/lib_error_examples/blob/main/lifecycle/main.c).

## Verification boundary

Run `scripts/runtime/student-workflow.sh -- <command>`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
