# Keep command evidence bounded, deterministic, and private

These examples cover command-level behavior that C compilers and source analyzers cannot infer. Each section gives a broken execution contract, its expected diagnostic, a repaired contract, and the expected clean result.

<a id="P101-TEST-DETERMINISM-001"></a>

## P101-TEST-DETERMINISM-001 — identical inputs produce different outputs

Broken input:

```text
run 1: exit=0 stdout="record order A,B"
run 2: exit=0 stdout="record order B,A"
```

Expected diagnostic:

```text
P101-TEST-DETERMINISM-001: identical admitted inputs produced different exit status or output bytes
```

Repaired input:

```text
sort records by their stable semantic identity before serialization
```

Expected clean result:

```text
No P101-TEST-DETERMINISM-001 finding is emitted for the repaired command.
```

<a id="P101-DATA-001"></a>

## P101-DATA-001 — sensitive data reaches command output

Broken input:

```text
diagnostic: authentication failed for token=p101-secret-probe
```

Expected diagnostic:

```text
P101-DATA-001: command output disclosed the admitted sensitive probe
```

Repaired input:

```text
diagnostic: authentication failed for token=<redacted>
```

Expected clean result:

```text
No P101-DATA-001 finding is emitted for the repaired command.
```

<a id="P101-RESOURCE-006"></a>

## P101-RESOURCE-006 — command output exceeds its resource budget

Broken input:

```text
admitted output budget=4096 bytes; observed output=65536 bytes
```

Expected diagnostic:

```text
P101-RESOURCE-006: command output exceeded its admitted byte budget
```

Repaired input:

```text
bound findings, summarize overflow, and retain the complete evidence in an explicitly admitted artifact
```

Expected clean result:

```text
No P101-RESOURCE-006 finding is emitted while the command remains within its declared budget.
```

<a id="P101-TIME-001"></a>

## P101-TIME-001 — command exceeds a monotonic elapsed-time budget

Broken input:

```text
admitted elapsed-time budget=50 ms; observed command does not complete within that budget
```

Expected diagnostic:

```text
P101-TIME-001: command exceeded its admitted monotonic elapsed-time budget
```

Repaired input:

```text
remove repeated work or admit a justified larger budget measured with the monotonic clock
```

Expected clean result:

```text
No P101-TIME-001 finding is emitted while the command remains within its declared monotonic budget.
```

<a id="P101-TEST-RECOVERY-001"></a>

## P101-TEST-RECOVERY-001 — caller recovery is not demonstrated

Broken input:

```text
admitted injected-fault scenario expects exit=0; observed exit=7
```

Expected diagnostic:

```text
P101-TEST-RECOVERY-001: the admitted fault scenario did not produce the caller's expected recovery status
```

Repaired input:

```text
the application observes the wrapper error, performs caller-owned recovery, and returns the declared status
```

Expected clean result:

```text
No P101-TEST-RECOVERY-001 finding is emitted for the admitted injected-fault scenario.
```

<a id="P101-SCHEMA-001"></a>

## P101-SCHEMA-001 — a retained schema fixture is no longer accepted

Broken input:

```text
the tool rejects a retained older-version fixture that its compatibility contract still admits
```

Expected diagnostic:

```text
P101-SCHEMA-001: the admitted compatibility fixture did not produce its expected status
```

Repaired input:

```text
retain a bounded decoder path for the admitted schema version; the fixture command asserts the normalized semantic fields before returning its expected status
```

Expected clean result:

```text
No P101-SCHEMA-001 finding is emitted for every retained compatibility fixture.
```

<a id="P101-TEST-SIDE-EFFECT-001"></a>

## P101-TEST-SIDE-EFFECT-001 — repeated runs produce different declared filesystem effects

Broken input:

```text
Two runs return the same status and output, but their admitted effect-manifest bytes differ.
```

Expected diagnostic:

```text
P101-TEST-SIDE-EFFECT-001: identical admitted inputs produced different declared filesystem-effect manifest bytes
```

Repaired input:

```text
Make generated paths and bytes deterministic, then write the same declared effect manifest on both runs.
```

Expected clean result:

```text
No P101-TEST-SIDE-EFFECT-001 finding is emitted when status, output, and the declared manifest are identical.
```

The command runner compares only the manifest path supplied by the caller. It does not claim to observe undeclared filesystem changes.

## Platform boundary

The command-contract runner observes exit status and captured bytes. Recovery and compatibility modes verify only scenarios and fixtures explicitly supplied by the owning test; a compatibility command must assert decoded semantic fields rather than merely report that parsing returned success. It does not prove determinism for inputs, schedules, environment variables, or platforms that were not held constant, and the byte budget is not a CPU- or memory-complexity proof.
Redaction mode must use a synthetic probe, never a live credential: the probe is intentionally supplied as a command-line argument so the runner can search the complete binary output stream.

## Correct reference

See [structured record fields with explicit serialization inputs](https://github.com/programming101dev/lib_record_examples/blob/main/fields/main.c).

## Verification boundary

Run `test-command-contract` with `determinism`, `redaction`, `output-limit`, `time-limit`, `recovery`, or `schema-compatibility`. The owning p101-test suite demonstrates both rejected and clean executions.
