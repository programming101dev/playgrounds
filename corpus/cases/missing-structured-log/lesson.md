# Use structured log fields

`bad password` is understandable to a human in the moment, but it is poor
operational evidence. It has no stable event name, no severity, and no outcome.

The fixed version should use fields such as:

```text
severity=warning event=login_failed outcome=denied
```

That makes the log searchable, testable, and easier to aggregate across many
runs or many students.

Canonical anchors: OWASP Logging Cheat Sheet, NIST SSDF.
