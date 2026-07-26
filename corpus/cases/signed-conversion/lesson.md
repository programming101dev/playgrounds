# Do not convert negative counts to unsigned sizes

`size_t` cannot represent negative values. If a negative signed value is
converted to `size_t`, it becomes a large positive value instead of staying
negative.

The fixed version should validate while the value is still signed, reject `-1`,
and only then convert a known-good value.

Canonical anchors: CWE-195, CWE-681, CERT INT rules.
