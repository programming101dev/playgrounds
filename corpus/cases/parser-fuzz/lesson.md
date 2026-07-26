# Fuzz boundary-heavy parsers

The broken fixture records `fuzz_target=missing`. Parsers are where strange
inputs pile up: empty strings, huge lengths, embedded NUL bytes, repeated
separators, and malformed records.

The fixed version should add a fuzz target or boundary-heavy test harness.

Canonical anchors: NIST SSDF verification practices.
