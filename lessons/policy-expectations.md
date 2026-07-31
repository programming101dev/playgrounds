# Turn observed behavior into an executable expectation

A clean run is not necessarily the intended run. An expectation records the
observable relationships that must hold: required or forbidden findings,
resource ownership, calls, and causal edges.

Capture a known-good behavior, write the narrowest stable expectation, then
verify future analyses against it. Avoid volatile values such as process IDs,
descriptor numbers, addresses, and timestamps unless they are the behavior
under test.

Verify with:

```sh
p101 verify -e expectations.txt analysis-directory
```

An expectation proves only that the admitted model satisfies the declared
rules. Missing instrumentation remains a blind spot.
