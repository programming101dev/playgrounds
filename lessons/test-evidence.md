# Keep test evidence complete and replayable

A passing command is useful only when the receipt says what was tested. Wrapper
conformance therefore joins the public API inventory, generated fault cases,
instrumentation observations, platform fault catalog, and native outcomes by
stable identities. A missing row is not treated as success.

`P101-TEST-CONFORMANCE-001` means the conformance tool could not load or write
its admitted evidence. `P101-TEST-CONFORMANCE-002` means a wrapper is present
but its required test, fault, instrumentation, or result evidence is missing.
`P101-TEST-RECEIPT-001` means repository test results could not be normalized
into the governed receipt.

## Repair workflow

1. Read the first diagnostic and identify the named input or wrapper identity.
2. Repair the producer or add the missing test; do not synthesize a passing row.
3. Re-run the owning repository test so it emits fresh evidence.
4. Re-run wrapper conformance with the same platform and compiler lane.
5. Keep the receipt with the inputs it summarizes.

The conformance join proves completeness only for the admitted manifests and
observations. It cannot see unlisted APIs, unexecuted native paths, or results
discarded before the receipt was written.
