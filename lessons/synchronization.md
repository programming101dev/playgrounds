# Reason about lock, wait, and join graphs

Concurrency failures are relationships, not isolated calls. A lock-order cycle
means different paths acquire locks in incompatible orders. A live wait-for
cycle means each participant is blocked on another. A join cycle means threads
cannot all finish.

Choose one global lock order and apply it everywhere. Keep critical sections
small, make condition predicates explicit, and never join while holding a lock
the target needs. Capacity or stream-integrity findings mean the analysis is
incomplete; fix those before trusting a clean result.

Verify with repeated and adversarial schedules:

```sh
p101 check -- ./your-multithreaded-program
```

Observed schedules cannot prove the absence of races or deadlocks. Combine the
result with ThreadSanitizer and focused stress tests.
