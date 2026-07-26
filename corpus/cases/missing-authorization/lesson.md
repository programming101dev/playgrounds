# Check authorization after authentication

The broken fixture records `authenticated=true` and `authorized=false`, but the
action is still accepted.

The fixed version should check whether the authenticated user is allowed to
perform the requested action.

Canonical anchors: CWE-862, CWE-863.
