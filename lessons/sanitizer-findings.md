# Use sanitizer evidence alongside wrapper evidence

The p101 event stream explains operations that passed through p101 wrappers.
Compiler sanitizers observe a different boundary: invalid memory access,
undefined behavior, leaks, and data races in instrumented code, including code
that did not emit a p101 event.

The two sources complement each other. A resource report can explain which
wrapper acquired an object, while a sanitizer can show where the program later
used invalid memory. Neither report proves that unexecuted paths are safe.

## Findings

- `P101-SAN-001` is an AddressSanitizer memory-access failure.
- `P101-SAN-002` is a LeakSanitizer leak report.
- `P101-SAN-003` is an UndefinedBehaviorSanitizer runtime error.
- `P101-SAN-004` is a ThreadSanitizer race or synchronization report.

## Repair workflow

1. Start with the first sanitizer finding, because later reports may be
   consequences of the first corrupted state.
2. Open the reported source location and identify the lifetime, bounds, or
   synchronization invariant that was violated.
3. Fix ownership or synchronization rather than suppressing the diagnostic.
4. Rebuild with the same sanitizer set.
5. Replay the exact command with `p101-inspect run` and confirm that the sanitizer and
   p101 policy reports are both clean.

Sanitizer output is execution evidence. Add a focused regression test for the
repaired path so the fix does not depend on manually repeating one run.
