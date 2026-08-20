# Keep test evidence complete and replayable

These examples cover missing or malformed evidence in governed test receipts.
Each section gives a broken input, its expected diagnostic, a repaired input, and the expected clean result.
Canonical correct programs remain in the corresponding example repository; native detection evidence remains in the owning tool suite.

<a id="P101-TEST-CONFORMANCE-001"></a>

## P101-TEST-CONFORMANCE-001 — conformance evidence cannot be loaded or written

Broken input:

```text
required manifest is missing or malformed
```

Expected diagnostic:

```text
P101-TEST-CONFORMANCE-001: conformance evidence cannot be loaded or written
```

Repaired input:

```text
manifest schema=p101-wrapper-conformance-v1 status=complete
```

Expected clean result:

```text
No P101-TEST-CONFORMANCE-001 finding is emitted for the repaired input.
```

<a id="P101-TEST-CONFORMANCE-002"></a>

## P101-TEST-CONFORMANCE-002 — wrapper lacks required conformance evidence

Broken input:

```text
wrapper exists but its native fault result is absent
```

Expected diagnostic:

```text
P101-TEST-CONFORMANCE-002: wrapper lacks required conformance evidence
```

Repaired input:

```text
wrapper identity has native invocation and every declared fault outcome
```

Expected clean result:

```text
No P101-TEST-CONFORMANCE-002 finding is emitted for the repaired input.
```

<a id="P101-TEST-RECEIPT-001"></a>

## P101-TEST-RECEIPT-001 — repository test result cannot be normalized

Broken input:

```text
test command has no parseable governed result
```

Expected diagnostic:

```text
P101-TEST-RECEIPT-001: repository test result cannot be normalized
```

Repaired input:

```text
repository result=PASS exit=0 receipt_schema=p101-repository-test-v1
```

Expected clean result:

```text
No P101-TEST-RECEIPT-001 finding is emitted for the repaired input.
```

## Platform boundary

Conformance is evaluated separately against each platform's declared wrapper faults and native outcomes; evidence from one platform cannot satisfy another platform's row.

## Correct reference

See [a CMake test that executes the built example](https://github.com/programming101dev/lib_transition_examples/blob/main/test/CMakeLists.txt).

## Verification boundary

Run `cmake --build <build-directory> --target p101-test_test_all`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
