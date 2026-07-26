# Validate external input at the boundary

Validation is most effective at the boundary where external text becomes program
state. The broken fixture accepts a path containing `..`, then logs it as an
accepted export target.

The fixed version should reject the value and make that decision observable:

```text
event=file_export outcome=rejected
```

Canonical anchors: CWE-20, CWE-22, CERT API00-C.
