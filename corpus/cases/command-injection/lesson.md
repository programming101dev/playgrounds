# Do not build shell commands from untrusted text

This fixture does not execute a shell command. It logs the command string that a
bad implementation would be tempted to execute.

The dangerous part is the semicolon:

```text
; rm -rf /
```

A safer design keeps data out of shell syntax: use an argv array, validate each
argument, or avoid invoking a shell entirely.

Canonical anchors: CWE-78, CWE-77.
