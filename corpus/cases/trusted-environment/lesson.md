# Do not trust environment variables for security decisions

The broken fixture records `variable=PATH` being used for execution. Environment
variables are external input.

The fixed version should avoid trusting environment-controlled paths for
sensitive actions, or validate and constrain the value before use.

Canonical anchors: CWE-426/CWE-427, CERT ENV guidance.
