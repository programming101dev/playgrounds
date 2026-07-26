# Neutralize log injection

Structured logs depend on one event meaning one record. If untrusted input can
contain a newline, the attacker can forge a second record.

The broken fixture lets the username create a fake success event:

```text
event=admin_login outcome=success
```

The fix is not to stop logging the username. The fix is to neutralize control
characters before they reach the log sink, for example by writing `\n` as two
visible characters.

Canonical anchors: OWASP Logging Cheat Sheet, CWE-117.
