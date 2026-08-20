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

## Platform boundary

Platform-loss findings compare the explicit macOS, Linux, and FreeBSD support fields in two manifests; they do not infer support from the host running the comparison.

## Correct reference

See [a consumer using the stable `lib_transition` public interface](https://github.com/programming101dev/lib_transition_examples/blob/main/table/main.c).

## Verification boundary

Run `scripts/tests/test-p101-api-diff.sh`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
