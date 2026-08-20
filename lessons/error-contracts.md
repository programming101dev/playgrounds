# Make environment and error ownership explicit

These examples cover explicit environment, error, control-flow, and call-observability contracts.
Each section gives a broken input, its expected diagnostic, a repaired input, and the expected clean result.
Canonical correct programs remain in the corresponding example repository; native detection evidence remains in the owning tool suite.

<a id="P101-ERR-001"></a>

## P101-ERR-001 — p101 operation has no visible environment contract

Broken input:

```text
value = p101_operation(err);
```

Expected diagnostic:

```text
P101-ERR-001: p101 operation has no visible environment contract
```

Repaired input:

```text
value = p101_operation(env, err);
```

Expected clean result:

```text
No P101-ERR-001 finding is emitted for the repaired input.
```

<a id="P101-ERR-002"></a>

## P101-ERR-002 — fallible operation has no visible error contract

Broken input:

```text
value = p101_operation(env);
```

Expected diagnostic:

```text
P101-ERR-002: fallible operation has no visible error contract
```

Repaired input:

```text
value = p101_operation(env, err);
```

Expected clean result:

```text
No P101-ERR-002 finding is emitted for the repaired input.
```

<a id="P101-ERR-003"></a>

## P101-ERR-003 — fallible wrapper discards its error object

Broken input:

```text
value = p101_operation(env, NULL);
```

Expected diagnostic:

```text
P101-ERR-003: fallible wrapper discards its error object
```

Repaired input:

```text
value = p101_operation(env, err);
```

Expected clean result:

```text
No P101-ERR-003 finding is emitted for the repaired input.
```

<a id="P101-ERR-004"></a>

## P101-ERR-004 — second fallible call can obscure the first failure

Broken input:

```text
first = p101_first(env, err);
second = p101_second(env, err);
```

Expected diagnostic:

```text
P101-ERR-004: second fallible call can obscure the first failure
```

Repaired input:

```text
first = p101_first(env, err);
failed = p101_error_has_error(err);
if(!failed)
{
    second = p101_second(env, err);
}
```

Expected clean result:

```text
No P101-ERR-004 finding is emitted for the repaired input.
```

<a id="P101-ERR-005"></a>

## P101-ERR-005 — created error object is not destroyed

Broken input:

```text
err = p101_error_create();
return status;
```

Expected diagnostic:

```text
P101-ERR-005: created error object is not destroyed
```

Repaired input:

```text
err = p101_error_create();
/* work */
p101_error_destroy(&err);
return status;
```

Expected clean result:

```text
No P101-ERR-005 finding is emitted for the repaired input.
```

<a id="P101-ERR-006"></a>

## P101-ERR-006 — created environment is not destroyed

Broken input:

```text
env = p101_env_create(err);
return status;
```

Expected diagnostic:

```text
P101-ERR-006: created environment is not destroyed
```

Repaired input:

```text
env = p101_env_create(err);
/* work */
p101_env_destroy(&env);
return status;
```

Expected clean result:

```text
No P101-ERR-006 finding is emitted for the repaired input.
```

<a id="P101-ERR-007"></a>

## P101-ERR-007 — helper terminates the process

Broken input:

```text
static void helper(void)
{
    exit(EXIT_FAILURE);
}
```

Expected diagnostic:

```text
P101-ERR-007: helper terminates the process
```

Repaired input:

```text
static int helper(struct p101_error *err)
{
    int status = -1;
    P101_ERROR_RAISE_USER(err, "helper failed", EINVAL);
    return status;
}
```

Expected clean result:

```text
No P101-ERR-007 finding is emitted for the repaired input.
```

<a id="P101-ERR-008"></a>

## P101-ERR-008 — function has more than one exit point

Broken input:

```text
if(failed)
{
    return -1;
}
return 0;
```

Expected diagnostic:

```text
P101-ERR-008: function has more than one exit point
```

Repaired input:

```text
status = 0;
if(failed)
{
    status = -1;
}
return status;
```

Expected clean result:

```text
No P101-ERR-008 finding is emitted for the repaired input.
```

<a id="P101-ERR-009"></a>

## P101-ERR-009 — call is embedded in a larger expression

Broken input:

```text
if(p101_ready(env, err))
{
    use(p101_value(env, err));
}
```

Expected diagnostic:

```text
P101-ERR-009: call is embedded in a larger expression
```

Repaired input:

```text
ready = p101_ready(env, err);
if(ready)
{
    value = p101_value(env, err);
    use(value);
}
```

Expected clean result:

```text
No P101-ERR-009 finding is emitted for the repaired input.
```

<a id="P101-ERR-010"></a>

## P101-ERR-010 — fallible output is consumed before its error is checked

Broken input:

```text
value = p101_read_value(env, err);
consume(value);
failed = p101_error_has_error(err);
```

Expected diagnostic:

```text
P101-ERR-010: fallible output is consumed before its error is checked
```

Repaired input:

```text
value = p101_read_value(env, err);
failed = p101_error_has_error(err);
if(!failed)
{
    consume(value);
}
```

Expected clean result:

```text
No P101-ERR-010 finding is emitted for the repaired input.
```

<a id="P101-ERR-011"></a>

## P101-ERR-011 — a semantic must-check result is discarded

Broken input:

```text
operation_with_must_check_contract();
```

The declaration carries the `p101:result:must-check` semantic role. This p101 rule is reserved for a project contract that is not already enforced by ordinary compiler unused-result diagnostics.

Expected diagnostic:

```text
P101-ERR-011: the result of an operation with the p101:result:must-check role was discarded
```

Repaired input:

```text
status = operation_with_must_check_contract();
if(status != 0) { handle_failure(status); }
```

Expected clean result:

```text
No P101-ERR-011 finding is emitted for the repaired input.
```

<a id="P101-ERR-012"></a>

## P101-ERR-012 — cleanup can replace the primary error

Broken input:

```text
failed = p101_error_has_error(err);
if(failed) { fallible_cleanup(env, err); }
```

Expected diagnostic:

```text
P101-ERR-012: fallible cleanup reused the primary p101_error object
```

Repaired input:

```text
failed = p101_error_has_error(err);
if(failed) { fallible_cleanup(env, P101_ERROR_OPTIONAL); }
```

Expected clean result:

```text
No P101-ERR-012 finding is emitted for the repaired input.
```

This check is admitted only for cleanup declarations carrying the semantic role `p101:cleanup:fallible`; it does not infer cleanup from a function name.

## Platform boundary

These contracts are checked from each platform's parsed C declarations and definitions; absence of source for a platform is not evidence of compliance.

## Correct references

- [Create, use, and destroy a `p101_error`](https://github.com/programming101dev/lib_error_examples/blob/main/lifecycle/main.c).
- [Create and destroy a `p101_env`](https://github.com/programming101dev/lib_env_examples/blob/main/env/get_tracer/main.c).

## Verification boundary

Run `programs/p101-audit/audit-doctor -s <source-path>`. The example explains the repair; the owning tool suite proves the diagnostic behavior.
A clean result is bounded by the files, events, manifests, and platform evidence admitted by that tool.
