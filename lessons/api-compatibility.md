# Review public API changes before consumers discover them

These examples cover incompatible changes between two public API manifests.
Each section gives a broken input, its expected diagnostic, a repaired input, and the expected clean result.
Canonical correct programs remain in the corresponding example repository; native detection evidence remains in the owning tool suite.

<a id="P101-API-001"></a>

## P101-API-001 — public API is removed

Broken input:

```text
before contains p101_widget_open; after does not
```

Expected diagnostic:

```text
P101-API-001: public API is removed
```

Repaired input:

```text
before contains p101_widget_open; after contains p101_widget_open
```

Expected clean result:

```text
No P101-API-001 finding is emitted for the repaired input.
```

<a id="P101-API-002"></a>

## P101-API-002 — public API moves to another library

Broken input:

```text
p101_widget_open moves from lib_widget to lib_util
```

Expected diagnostic:

```text
P101-API-002: public API moves to another library
```

Repaired input:

```text
p101_widget_open remains owned by lib_widget
```

Expected clean result:

```text
No P101-API-002 finding is emitted for the repaired input.
```

<a id="P101-API-003"></a>

## P101-API-003 — public API moves to another header

Broken input:

```text
p101_widget_open moves from widget.h to util.h
```

Expected diagnostic:

```text
P101-API-003: public API moves to another header
```

Repaired input:

```text
p101_widget_open remains declared by widget.h
```

Expected clean result:

```text
No P101-API-003 finding is emitted for the repaired input.
```

<a id="P101-API-004"></a>

## P101-API-004 — public API loses platform support

Broken input:

```text
freebsd=true becomes freebsd=false
```

Expected diagnostic:

```text
P101-API-004: public API loses platform support
```

Repaired input:

```text
freebsd=true remains freebsd=true
```

Expected clean result:

```text
No P101-API-004 finding is emitted for the repaired input.
```

<a id="P101-API-005"></a>

## P101-API-005 — public API function type changes

Broken input:

```text
int p101_widget_open(const char *path);
becomes
long p101_widget_open(char *path);
```

Expected diagnostic:

```text
P101-API-005: public API Clang-observed function type changed
```

Repaired input:

```text
Keep the existing declaration and introduce a separately named operation for the new contract.
```

Expected clean result:

```text
No P101-API-005 finding is emitted for the repaired input.
```

<a id="P101-API-006"></a>

## P101-API-006 — public ownership semantics change

Broken input:

```text
an API annotated as acquiring caller-owned state becomes a release operation or loses the annotation
```

Expected diagnostic:

```text
P101-API-006: public API ownership semantic role changed or was removed
```

Repaired input:

```text
Keep the existing acquire/release role and introduce a separately named API for the different ownership contract.
```

Expected clean result:

```text
No P101-API-006 finding is emitted when the declared semantic ownership role remains stable.
```

<a id="P101-API-007"></a>

## P101-API-007 — public struct or union layout changes

Broken input:

```text
The Clang-observed size or alignment of a public struct/union changes on the same platform.
```

Expected diagnostic:

```text
P101-API-007: public struct or union size/alignment changed on this platform
```

Repaired input:

```text
Keep the published layout stable, or introduce a new opaque/versioned type.
```

Expected clean result:

```text
No P101-API-007 finding is emitted when the platform-qualified layout stays stable.
```

<a id="P101-API-008"></a>

## P101-API-008 — public enumerator value changes

Broken input:

```text
P101_MODE_READY = 2 becomes P101_MODE_READY = 3
```

Expected diagnostic:

```text
P101-API-008: public enumerator value changed
```

Repaired input:

```text
Preserve the existing numeric value and add a new enumerator for new behavior.
```

Expected clean result:

```text
No P101-API-008 finding is emitted when numeric values remain stable.
```

<a id="P101-API-009"></a>

## P101-API-009 — exported object-like macro value changes

Broken input:

```text
#define P101_LIMIT 64 becomes #define P101_LIMIT 32
```

Expected diagnostic:

```text
P101-API-009: exported object-like macro value changed
```

Repaired input:

```text
Keep the published constant value or introduce a separately named constant.
```

Expected clean result:

```text
No P101-API-009 finding is emitted when the admitted macro replacement stays stable.
```

## Platform boundary

Platform-loss findings compare the explicit macOS, Linux, and FreeBSD support fields in two manifests. Function-type and ownership-role findings use the shared Clang fact bundle. Neither check infers support, data layout, visibility, or ownership properties absent from those inputs.

## Correct reference

See [a consumer using the stable `lib_transition` public interface](https://github.com/programming101dev/lib_transition_examples/blob/main/table/main.c).

## Verification boundary

Run `scripts/tests/test-p101-api-diff.sh`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
