# Reduce module coupling and public API surface

Start with file scope. Functions, types, and macros used by only one translation
unit should stay private, and private functions should normally be `static`.
Headers should expose only declarations required by another module.

Break dependency cycles by moving the shared concept to a narrower module or by
reversing control through a callback. Avoid dumping unrelated helpers into
`util.c`; name a module after the responsibility it owns. Treat size thresholds
as prompts for judgment rather than automatic proof of bad design.

Verify with:

```sh
p101 module-map src include
```

Library mode cannot decide whether an exported symbol is unused by external
consumers. Use the workspace-wide audit before removing public APIs.
