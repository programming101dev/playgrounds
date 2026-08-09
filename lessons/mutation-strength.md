# Measure whether tests reject wrong behavior

Coverage shows that code executed. Mutation testing asks whether the tests
notice when that code is made wrong. A surviving mutant is evidence that the
test suite did not distinguish the original behavior from a specific plausible
mistake.

Read the mutation, add the smallest behavioral assertion that should kill it,
and rerun the ordinary tests before rerunning mutation analysis. Do not add a
test that merely mirrors the implementation.

Verify with:

```sh
../programs/p101-test/test-mutation --test-command './test.sh' src
```

The available mutation operators are a sample of mistakes, not proof that the
test suite detects every defect.
