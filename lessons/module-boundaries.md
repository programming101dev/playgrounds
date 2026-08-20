# Reduce module coupling and public API surface

These examples cover AST-backed module shape and the few explicitly lexical C interface conventions.
Each section gives a broken input, its expected diagnostic, a repaired input, and the expected clean result.
Canonical correct programs remain in the corresponding example repository; native detection evidence remains in the owning tool suite.

<a id="P101-MOD-002"></a>

## P101-MOD-002 — module owns too many functions

Broken input:

```text
one.c owns parsing, storage, networking, and reporting
```

Expected diagnostic:

```text
P101-MOD-002: module owns too many functions
```

Repaired input:

```text
Split one responsibility into a module with a narrow interface.
```

Expected clean result:

```text
No P101-MOD-002 finding is emitted for the repaired input.
```

<a id="P101-MOD-003"></a>

## P101-MOD-003 — module exposes too much public API

Broken input:

```text
helper functions are non-static and exported
```

Expected diagnostic:

```text
P101-MOD-003: module exposes too much public API
```

Repaired input:

```text
Make file-local helpers static and expose only consumer-required declarations.
```

Expected clean result:

```text
No P101-MOD-003 finding is emitted for the repaired input.
```

<a id="P101-MOD-004"></a>

## P101-MOD-004 — entrypoint owns subsystem logic

Broken input:

```text
main.c contains parsing, business logic, and persistence
```

Expected diagnostic:

```text
P101-MOD-004: entrypoint owns subsystem logic
```

Repaired input:

```text
Keep main responsible for wiring, top-level status, and cleanup.
```

Expected clean result:

```text
No P101-MOD-004 finding is emitted for the repaired input.
```

<a id="P101-MOD-006"></a>

## P101-MOD-006 — private helper has external linkage

Broken input:

```text
int helper(void) { return 0; }
```

Expected diagnostic:

```text
P101-MOD-006: private helper has external linkage
```

Repaired input:

```text
static int helper(void) { return 0; }
```

Expected clean result:

```text
No P101-MOD-006 finding is emitted for the repaired input.
```

<a id="P101-MOD-007"></a>

## P101-MOD-007 — header declaration has no definition

Broken input:

```text
int public_operation(void);
```

Expected diagnostic:

```text
P101-MOD-007: header declaration has no definition
```

Repaired input:

```text
Provide the matching non-static definition or remove the declaration.
```

Expected clean result:

```text
No P101-MOD-007 finding is emitted for the repaired input.
```

<a id="P101-MOD-008"></a>

## P101-MOD-008 — declared interface is unused

Broken input:

```text
public.h declares an operation no scanned consumer calls
```

Expected diagnostic:

```text
P101-MOD-008: declared interface is unused
```

Repaired input:

```text
Remove the unused interface or add the real admitted consumer.
```

Expected clean result:

```text
No P101-MOD-008 finding is emitted for the repaired input.
```

<a id="P101-MOD-009"></a>

## P101-MOD-009 — public macro is unused

Broken input:

```text
#define PROJECT_LIMIT 32
```

Expected diagnostic:

```text
P101-MOD-009: public macro is unused
```

Repaired input:

```text
Keep the macro private or use it through the public contract that needs it.
```

Expected clean result:

```text
No P101-MOD-009 finding is emitted for the repaired input.
```

<a id="P101-MOD-010"></a>

## P101-MOD-010 — public type is unused

Broken input:

```text
typedef struct project_unused project_unused;
```

Expected diagnostic:

```text
P101-MOD-010: public type is unused
```

Repaired input:

```text
Remove the public type or add the consumer that establishes its purpose.
```

Expected clean result:

```text
No P101-MOD-010 finding is emitted for the repaired input.
```

<a id="P101-MOD-011"></a>

## P101-MOD-011 — local include graph contains a cycle

Broken input:

```text
a.h includes b.h; b.h includes a.h
```

Expected diagnostic:

```text
P101-MOD-011: local include graph contains a cycle
```

Repaired input:

```text
Extract the shared declaration or invert one dependency.
```

Expected clean result:

```text
No P101-MOD-011 finding is emitted for the repaired input.
```

<a id="P101-MOD-012"></a>

## P101-MOD-012 — local include target is missing

Broken input:

```text
#include "missing.h"
```

Expected diagnostic:

```text
P101-MOD-012: local include target is missing
```

Repaired input:

```text
Include the owning module's existing public header.
```

Expected clean result:

```text
No P101-MOD-012 finding is emitted for the repaired input.
```

<a id="P101-MOD-013"></a>

## P101-MOD-013 — include violates the declared layer graph

Broken input:

```text
presentation includes storage internals directly
```

Expected diagnostic:

```text
P101-MOD-013: include violates the declared layer graph
```

Repaired input:

```text
Route the dependency through an allowed interface or update intentional policy.
```

Expected clean result:

```text
No P101-MOD-013 finding is emitted for the repaired input.
```

<a id="P101-MOD-014"></a>

## P101-MOD-014 — create vocabulary lacks destroy pair

Broken input:

```text
widget_create exists without widget_destroy
```

Expected diagnostic:

```text
P101-MOD-014: create vocabulary lacks destroy pair
```

Repaired input:

```text
Add the matching destroy operation and make ownership explicit.
```

Expected clean result:

```text
No P101-MOD-014 finding is emitted for the repaired input.
```

<a id="P101-MOD-015"></a>

## P101-MOD-015 — collection accessor vocabulary is incomplete

Broken input:

```text
widget_count exists without widget_at
```

Expected diagnostic:

```text
P101-MOD-015: collection accessor vocabulary is incomplete
```

Repaired input:

```text
Provide the count/at pair or remove the partial collection interface.
```

Expected clean result:

```text
No P101-MOD-015 finding is emitted for the repaired input.
```

<a id="P101-MOD-016"></a>

## P101-MOD-016 — public function lacks its module prefix

Broken input:

```text
int open_widget(void);
```

Expected diagnostic:

```text
P101-MOD-016: public function lacks its module prefix
```

Repaired input:

```text
int widget_open(void);
```

Expected clean result:

```text
No P101-MOD-016 finding is emitted for the repaired input.
```

<a id="P101-MOD-017"></a>

## P101-MOD-017 — include guard names the wrong header

Broken input:

```text
widget.h uses OTHER_H
```

Expected diagnostic:

```text
P101-MOD-017: include guard names the wrong header
```

Repaired input:

```text
widget.h uses P101_WIDGET_WIDGET_H
```

Expected clean result:

```text
No P101-MOD-017 finding is emitted for the repaired input.
```

<a id="P101-MOD-018"></a>

## P101-MOD-018 — source does not include its own header first

Broken input:

```text
source includes dependencies before widget.h
```

Expected diagnostic:

```text
P101-MOD-018: source does not include its own header first
```

Repaired input:

```text
Include widget.h first so the public interface proves it is self-contained.
```

Expected clean result:

```text
No P101-MOD-018 finding is emitted for the repaired input.
```

<a id="P101-MOD-019"></a>

## P101-MOD-019 — lifecycle vocabulary lacks teardown

Broken input:

```text
widget_init exists without widget_deinit
```

Expected diagnostic:

```text
P101-MOD-019: lifecycle vocabulary lacks teardown
```

Repaired input:

```text
Add the matching teardown operation, even when it is currently small.
```

Expected clean result:

```text
No P101-MOD-019 finding is emitted for the repaired input.
```

<a id="P101-MOD-020"></a>

## P101-MOD-020 — name conversion is one-way

Broken input:

```text
widget_from_name exists without widget_name
```

Expected diagnostic:

```text
P101-MOD-020: name conversion is one-way
```

Repaired input:

```text
Provide both parse and print directions for the public vocabulary.
```

Expected clean result:

```text
No P101-MOD-020 finding is emitted for the repaired input.
```

<a id="P101-MOD-021"></a>

## P101-MOD-021 — public type uses a reserved suffix

Broken input:

```text
typedef struct widget widget_t;
```

Expected diagnostic:

```text
P101-MOD-021: public type uses a reserved suffix
```

Repaired input:

```text
Use a project-owned name such as struct p101_widget.
```

Expected clean result:

```text
No P101-MOD-021 finding is emitted for the repaired input.
```

<a id="P101-MOD-022"></a>

## P101-MOD-022 — environment and error parameters are out of contract order

Broken input:

```text
operation(value, err, env)
```

Expected diagnostic:

```text
P101-MOD-022: environment and error parameters are out of contract order
```

Repaired input:

```text
operation(env, err, value)
```

Expected clean result:

```text
No P101-MOD-022 finding is emitted for the repaired input.
```

<a id="P101-MOD-027"></a>

## P101-MOD-027 — allocation is sized by a repeated type

Broken input:

```text
item = malloc(sizeof(struct item));
```

Expected diagnostic:

```text
P101-MOD-027: allocation is sized by a repeated type
```

Repaired input:

```text
item = p101_malloc(env, err, sizeof(*item));
```

Expected clean result:

```text
No P101-MOD-027 finding is emitted for the repaired input.
```

## Platform boundary

Module findings use the declarations and definitions admitted for the current platform, while explicitly lexical rules remain limited to the source text they inspect.

## Correct reference

See the [small, prefixed `lib_transition` interface used from a separate program](https://github.com/programming101dev/lib_transition_examples/blob/main/table/main.c).

## Verification boundary

Run `programs/p101-audit/audit-modules <source-path>`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
