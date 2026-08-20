# Measure whether tests reject wrong behavior

This example covers a mutant that the current tests fail to reject.
Each section gives a broken input, its expected diagnostic, a repaired input, and the expected clean result.
Canonical correct programs remain in the corresponding example repository; native detection evidence remains in the owning tool suite.

<a id="P101-MUTATION-001"></a>

## P101-MUTATION-001 — mutant survives the test suite

Broken input:

```text
operator changed < to <= and all tests still pass
```

Expected diagnostic:

```text
P101-MUTATION-001: mutant survives the test suite
```

Repaired input:

```text
result = operation(boundary_value);
TEST_ASSERT_EQUAL_INT(expected_boundary_result, result);
```

Expected clean result:

```text
No P101-MUTATION-001 finding is emitted for the repaired input.
```

## Platform boundary

Mutation evidence is bounded by the selected source candidates and test command; surviving no generated mutant is not proof against mutations the tool did not generate.

## Correct reference

See [boundary assertions over a transition-table result](https://github.com/programming101dev/lib_transition_examples/blob/main/table/main.c).

## Verification boundary

Run `programs/p101-test/test-mutation --test-command '<test-command>' <source-path>`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
