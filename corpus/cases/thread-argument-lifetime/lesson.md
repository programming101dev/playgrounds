# Keep thread arguments alive until the thread is done

The broken fixture records a thread still running while its argument lives on the
caller stack. That becomes a lifetime bug as soon as the caller returns.

The fixed version should allocate owned argument storage, or join the thread
before the argument leaves scope.

Canonical anchors: CWE-416 shape, CERT CON/MEM lifetime guidance.
