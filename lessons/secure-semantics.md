# Make security-sensitive lifetime and callback obligations explicit

These examples cover semantic rules that are easy to miss in ordinary control-flow review. Each rule is based on resolved declarations, types, storage duration, or call relationships rather than variable spelling. The checks complement compiler warnings, clang-tidy, cppcheck, and sanitizers; they do not duplicate diagnostics those tools already provide reliably.

<a id="P101-THREAD-001"></a>

## P101-THREAD-001 — automatic storage escapes to a new thread

Broken input:

```c
static int launch_worker(void)
{
    struct work item;
    pthread_t   thread;
    int         status;

    status = pthread_create(&thread, NULL, run, &item);
    return status;
}
```

`item` ceases to exist when `launch_worker` returns, but the new thread can
still dereference its address. The check uses the argument's storage duration
and the semantic thread-creation role; the name `item` is irrelevant.

Expected diagnostic:

```text
P101-THREAD-001: automatic storage is passed to a thread whose lifetime can outlast the call
```

Repaired input:

```c
static int run_worker(struct work *item)
{
    pthread_t thread;
    int       create_status;
    int       join_status;
    int       result;

    result        = -1;
    create_status = pthread_create(&thread, NULL, run, item);
    if(create_status == 0)
    {
        join_status = pthread_join(thread, NULL);
        result      = join_status;
    }
    return result;
}
```

The caller owns `item` for the entire call, and the join prevents the worker
from outliving that contract. A heap-owned work item with an explicit ownership
transfer is another valid repair.

Expected clean result:

```text
No P101-THREAD-001 finding is emitted when the argument lifetime covers the thread lifetime.
```

<a id="P101-SIGNAL-001"></a>

## P101-SIGNAL-001 — signal handler accesses unsafe shared state

Broken input:

```c
static int stop;

static void handler(int signal_number)
{
    stop = signal_number;
}
```

An ordinary mutable object can be observed in an interrupted state. The check
follows the registered handler and its helpers, then classifies shared objects
by type and storage duration rather than by their spelling.

Expected diagnostic:

```text
P101-SIGNAL-001: signal-reachable code accesses mutable shared state that is not volatile sig_atomic_t or explicitly verified as a lock-free atomic
```

Repaired input:

```c
static volatile sig_atomic_t stop_requested;

static void handler(int signal_number)
{
    (void)signal_number;
    stop_requested = 1;
}
```

A verified lock-free atomic object is also admitted. Merely changing an
ordinary object's name or hiding the access in a helper does not satisfy the
rule.

Expected clean result:

```text
No P101-SIGNAL-001 finding is emitted for admitted signal-safe shared state.
```

<a id="P101-SIGNAL-002"></a>

## P101-SIGNAL-002 — signal handler reaches a non-async-signal-safe call

Broken input:

```c
static void report_signal(void)
{
    int print_status;

    print_status = printf("signal received\n");
    (void)print_status;
}

static void handler(int signal_number)
{
    (void)signal_number;
    report_signal();
}
```

`printf` is unsafe here even though it is hidden one call away from the
registered handler. The checker walks the semantic call graph transitively.

Expected diagnostic:

```text
P101-SIGNAL-002: signal-reachable code calls an operation outside the async-signal-safe set
```

Repaired input:

```c
static void handler(int signal_number)
{
    static const char message[] = "signal received\n";
    ssize_t           write_status;

    (void)signal_number;
    write_status = write(STDERR_FILENO, message, sizeof(message) - 1U);
    (void)write_status;
}
```

Keep recovery and formatted reporting in normal control flow. The handler may
only call operations in the portable async-signal-safe set.

Expected clean result:

```text
No P101-SIGNAL-002 finding is emitted when every signal-reachable call is in the portable safe set.
```

The accepted call set is the portable
[POSIX.1-2024 async-signal-safe table](https://pubs.opengroup.org/onlinepubs/9799919799/functions/V2_chap02.html),
not whichever additional functions happen to be safe on one host.

<a id="P101-ENV-001"></a>

## P101-ENV-001 — borrowed environment or locale pointer is used after invalidation

Broken input:

```c
value      = getenv("P101_MODE");
set_status = setenv("P101_MODE", "strict", 1);
length     = strlen(value);
```

The environment update can invalidate the borrowed pointer before `strlen`
uses it. The check relates the borrowed-result and invalidation roles to the
same semantic value flow.

Expected diagnostic:

```text
P101-ENV-001: a borrowed environment or locale pointer is used after an operation that can invalidate it
```

Repaired input:

```c
set_status = setenv("P101_MODE", "strict", 1);
value      = getenv("P101_MODE");
if(value != NULL)
{
    length = strlen(value);
}
```

Copying the value into owned storage before the update is also valid. Reusing
the old pointer after `setenv`, `putenv`, `unsetenv`, or the corresponding
locale invalidator is not.

Expected clean result:

```text
No P101-ENV-001 finding is emitted when the borrowed pointer is consumed before invalidation or reacquired afterward.
```

<a id="P101-FILE-001"></a>

## P101-FILE-001 — pathname check and use form a TOCTOU race

Broken input:

```c
check_status = access(path, R_OK);
if(check_status == 0)
{
    descriptor = open(path, O_RDONLY);
}
```

Another process can replace the pathname target between the check and use. The
checker follows the pathname object's semantic identity; it does not look for a
variable named `path`.

Expected diagnostic:

```text
P101-FILE-001: a pathname is checked and later used by a separate filesystem operation
```

Repaired input:

```c
descriptor = open(path, O_RDONLY);
if(descriptor == -1)
{
    error_number = errno;
}
```

When a directory relationship matters, prefer a descriptor-relative operation
such as `openat` and validate the opened object rather than predicting what a
future pathname lookup will find.

Expected clean result:

```text
No P101-FILE-001 finding is emitted when the decision and operation are not separated by a pathname race window.
```

<a id="P101-MEM-001"></a>

## P101-MEM-001 — allocation is requested with zero size

Broken input:

```c
allocation = malloc(0U);
```

Portable C permits an implementation-defined result for a zero-size request.
Code that treats the result as ordinary storage therefore has an unstable
contract.

Expected diagnostic:

```text
P101-MEM-001: allocation size is provably zero
```

Repaired input:

```c
allocation = NULL;
if(element_count != 0U)
{
    allocation = malloc(element_count * sizeof(*elements));
}
```

The static check diagnoses a provably zero request; runtime allocation events
cover a computed zero that was not constant in the source facts.

Expected clean result:

```text
No P101-MEM-001 finding is emitted when the empty case avoids the allocation call.
```

<a id="P101-MEM-002"></a>

## P101-MEM-002 — restricted copy operands overlap

Broken input:

```c
result = memcpy(buffer, buffer, length);
```

The restricted source and destination designate the same object, violating the
operation's contract even if a particular implementation appears to tolerate
it.

Expected diagnostic:

```text
P101-MEM-002: source and destination of a restricted copy are provably the same object
```

Repaired input:

```c
result = memmove(buffer + 1, buffer, length);
```

Use `memcpy` only after the design establishes disjoint ranges. Use `memmove`
when overlap is permitted by the operation.

Expected clean result:

```text
No P101-MEM-002 finding is emitted for disjoint restricted-copy operands or an overlap-safe operation.
```

<a id="P101-MOD-028"></a>

## P101-MOD-028 — recursion has no explicit bounded contract

Broken input:

```c
static size_t length(const struct node *node)
{
    size_t result;

    result = 0U;
    if(node != NULL)
    {
        result = 1U + length(node->next);
    }
    return result;
}
```

Input-controlled depth can exhaust the stack. The check uses the resolved call
graph, so renaming `length` or calling it through another local helper does not
make the recursion acceptable.

Expected diagnostic:

```text
P101-MOD-028: recursive call requires an explicit bounded-depth contract
```

Repaired input:

```c
static size_t length(const struct node *node)
{
    const struct node *current;
    size_t             result;

    current = node;
    result  = 0U;
    while(current != NULL)
    {
        result++;
        current = current->next;
    }
    return result;
}
```

If recursion is essential, its finite depth bound must be explicit, enforced,
and covered by boundary tests.

Expected clean result:

```text
No P101-MOD-028 finding is emitted after recursion is removed or admitted by an explicit bounded contract.
```

## Platform boundary

The static rules use the portable POSIX contract and only the declarations,
types, storage durations, and call edges visible in admitted translation units.
An implementation-specific extension does not weaken the cross-platform rule.

## Correct reference

Compare the repaired designs with the canonical
[thread lifetime](https://github.com/programming101dev/c-examples/blob/main/pthread/pthread_create-pthread_join/main.c),
[allocation lifetime](https://github.com/programming101dev/c-examples/blob/main/memory/malloc-free/main.c),
[memory copy](https://github.com/programming101dev/c-examples/blob/main/memory/memcpy/main.c),
[environment access](https://github.com/programming101dev/c-examples/blob/main/system-information/getenv/main.c),
and [open/close](https://github.com/programming101dev/c-examples/blob/main/files/open-close/main.c)
examples. Signal-safe behavior is defined by the linked POSIX table above.

## Verification boundary

The static checks see only admitted translation units and resolved AST identities. Exact constant sizes and direct object identities are diagnosed; runtime-dependent range overlap, unscanned callbacks, dynamically selected handlers, and third-party internals remain blind spots. A clean result is evidence for the scanned inputs, not a proof for every execution.
