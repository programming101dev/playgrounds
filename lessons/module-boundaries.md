# Reduce module coupling and public API surface

Start with file scope. Functions, types, and macros used by only one translation
unit should stay private, and private functions should normally be `static`.
Headers should expose only declarations required by another module. A
non-static function with no header declaration at all is clang-tidy's call
(`misc-use-internal-linkage`); the module map speaks up about the harder
question of whether anything actually uses the interface you did declare.

Break dependency cycles by moving the shared concept to a narrower module or by
reversing control through a callback. Avoid dumping unrelated helpers into
`util.c`; name a module after the responsibility it owns. Treat size thresholds
as prompts for judgment rather than automatic proof of bad design.

Name idioms carry the same weight as structure. Pair every `_create` with a
`_destroy` that takes the caller's pointer by address and nulls it, so a freed
handle cannot be used again. Expose collections as a `_count`/`_at` accessor
pair instead of leaking arrays. Give one module one vocabulary: every public
function shares the module's name prefix, so a call site names its owner. Open
every header with an include guard derived from the header's own name; a guard
that names a different file is a copy-paste bug waiting for its second
inclusion. Thin platform wrappers are the exception for pairing and prefixes:
they keep the platform's own names on purpose.

Structure follows the same discipline. A source file includes its own header —
first, in tool code — so the interface is proven to stand alone. A parser
implies a printer: `_from_name` without `_name` leaves a vocabulary nothing can
round-trip. `_init` pairs with `_deinit` and `_open` with `_close`, even when
today's teardown is empty, so call sites never change when the struct grows an
allocation. And names stay out of the implementation's namespace: leading
underscores and the POSIX `_t` suffix belong to the platform, so a name the
platform cannot take is a name that cannot break. clang-tidy owns the
leading-underscore half of that rule
(`bugprone-reserved-identifier`/`cert-dcl37-c`); the module map checks the `_t`
suffix, which no tool does.

Ownership belongs in names. A function that hands you memory says so —
`create`, `dup` — and a parameter that is merely borrowed is `const`. When a
callee assumes ownership, say `take` or `adopt`; when the caller keeps it, the
signature already says so. String producers take a caller-owned buffer and its
size rather than returning hidden allocations. Keep functions reentrant by
construction: no pointers to static storage, state threaded through parameters
— the reason serious code retired `strtok`. Give stateful structs a private
validity check and assert it at entry points; it turns representation bugs into
early, local failures. And when control flow grows a second `switch` over the
same enum, reach for a table: lib_fsm shows dispatch as data.

The evidence layer now judges shape as well as structure, straight from the
AST. The house signature reads env first, error second. A heap handle's
`_destroy` takes the caller's pointer by address and nulls it. A predicate
returns `bool`, because the type documents that there is no third answer; it
should also read as a question — `is_`, `has_`, `can_`, `should_` — though only
the return type is judged, because English has more ways to ask a question than
a token list can hold. Allocations size the object, not the type:
`malloc(sizeof(*p))` survives a retype.

Two habits belong here but are checked elsewhere, so the module map stays quiet
about them. Macro parameters wear parentheses and multi-statement macros wrap in
`do { } while(0)`, because the caller may pass `a + b` and because inside an
unbraced `if` only the first statement stays conditional — clang-tidy's
`bugprone-macro-parentheses` and `bugprone-multiple-statement-macro` report
both. And a signal handler calls only the async-signal-safe list and touches
only `volatile sig_atomic_t`, because everything else can deadlock a process
interrupted mid-operation — that is `bugprone-signal-handler`, also known as
`cert-sig30-c`. One problem earns one diagnostic; when a general tool already
finds it, the tool reports and the lesson explains.

Cross-module field access is worth thinking about, but no tool judges it here. A
record whose module hands out a lifecycle — `_create`, `_destroy`, `_init`,
`_deinit` — is an object: the lifecycle establishes invariants, and reaching
into its fields from another module goes around them, so prefer asking the
owning module. A record with no lifecycle is plain data, and reading its fields
across a module boundary is ordinary and correct; a shared model struct is a
deliberate pattern here, not a smell. The line between the two is a judgement
call often enough that it stays a habit to cultivate rather than a diagnostic to
enforce — this workspace's own tools reach into shared model records by design.

Verify with:

```sh
../programs/p101-audit/audit-modules src include
```

Library mode cannot decide whether an exported symbol is unused by external
consumers. Use the workspace-wide audit before removing public APIs.
