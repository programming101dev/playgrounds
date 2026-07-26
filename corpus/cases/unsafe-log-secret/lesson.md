# Do not log secrets

Good logging helps debugging and incident response, but logs are also copied,
indexed, backed up, and read by more people than the original program output.

The broken fixture writes:

```text
password=hunter2
```

That turns a failed login event into a credential leak. The fixed version should
keep the event structured and useful, but write a redacted field such as
`password=<redacted>`.

Canonical anchors: OWASP Logging Cheat Sheet, CWE-532, NIST SSDF.
