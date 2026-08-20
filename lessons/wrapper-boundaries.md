# Keep wrapper boundaries complete and observable

These examples cover calls that escape the p101 wrapper and instrumentation boundary.
Each section gives a broken input, its expected diagnostic, a repaired input, and the expected clean result.
Canonical correct programs remain in the corresponding example repository; native detection evidence remains in the owning tool suite.

<a id="P101-WRAP-001"></a>

## P101-WRAP-001 — native call bypasses an available wrapper

Broken input:

```text
read(fd, buffer, size);
```

Expected diagnostic:

```text
P101-WRAP-001: native call bypasses an available wrapper
```

Repaired input:

```text
result = p101_read(env, err, fd, buffer, size);
```

Expected clean result:

```text
No P101-WRAP-001 finding is emitted for the repaired input.
```

<a id="P101-WRAP-002"></a>

## P101-WRAP-002 — external call has no declared boundary

Broken input:

```text
vendor_operation(value);  /* no wrapper or allow rule */
```

Expected diagnostic:

```text
P101-WRAP-002: external call has no declared boundary
```

Repaired input:

```text
result = project_vendor_operation(env, err, value);
```

Expected clean result:

```text
No P101-WRAP-002 finding is emitted for the repaired input.
```

<a id="P101-WRAP-003"></a>

## P101-WRAP-003 — indirect call hides the native operation

Broken input:

```text
operation = read;
result = operation(fd, buffer, size);
```

Expected diagnostic:

```text
P101-WRAP-003: indirect call hides the native operation
```

Repaired input:

```text
result = p101_read(env, err, fd, buffer, size);
```

Expected clean result:

```text
No P101-WRAP-003 finding is emitted for the repaired input.
```

<a id="P101-WRAP-004"></a>

## P101-WRAP-004 — platform declaration is included through the wrong boundary

Broken input:

```text
#include <sys/socket.h>  /* duplicated private portability surface */
```

Expected diagnostic:

```text
P101-WRAP-004: platform declaration is included through the wrong boundary
```

Repaired input:

```text
#include <p101_network/sys/socket.h>
```

Expected clean result:

```text
No P101-WRAP-004 finding is emitted for the repaired input.
```

<a id="P101-WRAP-900"></a>

## P101-WRAP-900 — wrapper evidence cannot be parsed

Broken input:

```text
P101FACT	truncated-record
```

Expected diagnostic:

```text
P101-WRAP-900: wrapper evidence cannot be parsed
```

Repaired input:

```text
Regenerate a complete fact stream, then rerun the wrapper audit.
```

Expected clean result:

```text
No P101-WRAP-900 finding is emitted for the repaired input.
```

## Platform boundary

The rule applies to the APIs admitted on macOS, Linux, and FreeBSD; an API unavailable on a platform is not inferred from another platform's headers.

## Correct reference

See [`p101_abs` used through its wrapper boundary](https://github.com/programming101dev/lib_c_examples/blob/main/stdlib/abs/main.c).

## Verification boundary

Run `programs/p101-audit/audit-wrappers --strict-external <source-path>`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
