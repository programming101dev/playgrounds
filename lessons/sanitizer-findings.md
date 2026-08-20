# Use compiler runtime evidence to find unwrapped defects

These examples cover sanitizer reports admitted from an instrumented execution.
Each section gives a broken input, its expected diagnostic, a repaired input, and the expected clean result.
Canonical correct programs remain in the corresponding example repository; native detection evidence remains in the owning tool suite.

<a id="P101-SAN-001"></a>

## P101-SAN-001 — AddressSanitizer reports invalid memory access

Broken input:

```text
AddressSanitizer: heap-use-after-free
```

Expected diagnostic:

```text
P101-SAN-001: AddressSanitizer reports invalid memory access
```

Repaired input:

```text
pointer = allocate();
value = *pointer;
release(pointer);
```

Expected clean result:

```text
No P101-SAN-001 finding is emitted for the repaired input.
```

<a id="P101-SAN-002"></a>

## P101-SAN-002 — LeakSanitizer reports leaked memory

Broken input:

```text
LeakSanitizer: detected memory leaks
```

Expected diagnostic:

```text
P101-SAN-002: LeakSanitizer reports leaked memory
```

Repaired input:

```text
pointer = allocate();
release(pointer);
```

Expected clean result:

```text
No P101-SAN-002 finding is emitted for the repaired input.
```

<a id="P101-SAN-003"></a>

## P101-SAN-003 — UndefinedBehaviorSanitizer reports undefined behavior

Broken input:

```text
runtime error: signed integer overflow
```

Expected diagnostic:

```text
P101-SAN-003: UndefinedBehaviorSanitizer reports undefined behavior
```

Repaired input:

```text
if(left <= INT_MAX - right)
{
    result = left + right;
}
```

Expected clean result:

```text
No P101-SAN-003 finding is emitted for the repaired input.
```

<a id="P101-SAN-004"></a>

## P101-SAN-004 — ThreadSanitizer reports a race

Broken input:

```text
ThreadSanitizer: data race
```

Expected diagnostic:

```text
P101-SAN-004: ThreadSanitizer reports a race
```

Repaired input:

```text
lock(mutex);
shared_value = value;
unlock(mutex);
```

Expected clean result:

```text
No P101-SAN-004 finding is emitted for the repaired input.
```

## Platform boundary

All three platforms test normalization of admitted sanitizer output. This does not claim that every sanitizer is supplied or usable by every platform compiler.

## Correct reference

See [a matched allocation and release](https://github.com/programming101dev/c-examples/blob/main/memory/malloc-free/main.c).

## Verification boundary

Run `p101-inspect run -- <sanitized-command>`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
