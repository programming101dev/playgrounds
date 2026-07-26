# Avoid check-then-use races on paths

The broken fixture records `pattern=check_then_use`. Between the check and the
use, an attacker or another process can swap what the path refers to.

The fixed version should open first and verify through the descriptor when
possible.

Canonical anchors: CWE-367, CERT POS guidance.
