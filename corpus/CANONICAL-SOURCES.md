# Canonical C trap and security source list

The playground lessons should be grounded in widely recognized C and security
taxonomies. This file is the seed list for new labs: when a new pitfall is
added, map it to one or more of these sources and add a short lesson under
`corpus/cases/`.

There is no single canonical list of all C mistakes. The useful canon is a
crosswalk:

- secure C rules: CERT C and ISO/IEC TS 17961;
- vulnerability classes: MITRE CWE, especially memory, bounds, injection,
  resource, and information-exposure weaknesses;
- safer-subset rules: MISRA C and JPL/NASA-style safety-critical C rules;
- teaching traps: the comp.lang.c FAQ and compiler/static-analysis diagnostics;
- operational/security practices: OWASP logging guidance and NIST SSDF.

## Primary sources

| Source | Use in this playground |
| --- | --- |
| SEI CERT C Coding Standard | Secure-C rule IDs and teachable compliant/noncompliant pairs. |
| MITRE CWE | Vulnerability IDs and security framing for findings. |
| ISO/IEC TS 17961 | Diagnosable secure-C rules suitable for tool/lab checks. |
| MISRA C | Safer subset and analyzability rules for portability/reliability labs. |
| JPL/NASA Power-of-Ten style rules | Small, memorable safety-critical C habits: bounded control flow, narrow scope, checked returns. |
| comp.lang.c FAQ | Classic C misunderstandings: arrays vs pointers, null pointers, allocation, strings, declarations, stdio. |
| OWASP Logging Cheat Sheet | Application/security logging practices: what to log, what not to log, structured events, log-injection resistance. |
| NIST Secure Software Development Framework | Process-level practices: requirements, secure design, verification, vulnerability response, reproducibility. |

## Lab backlog by pitfall family

The status column is intentionally simple:

- `covered`: already represented by a corpus case.
- `candidate`: good next fixture.
- `tool`: better checked by a static tool than by a runtime playground case.

| Family | Canonical anchors | Playground status |
| --- | --- | --- |
| Descriptor leak on normal path | CWE-775, CERT FIO resource cleanup guidance | `covered`: `fd-leak` |
| Allocation leak on normal path | CWE-401, CERT MEM31-C | `covered`: `alloc-leak` |
| Error-path cleanup leak | CERT MEM12-C, CWE-772/CWE-775 | `covered`: `error-path-leak`, `early-return-*`, `partial-cleanup` |
| Double close / stale descriptor ownership | CWE-666, CERT FIO ownership rules | `covered`: `double-close` |
| Close/free of unowned resource | CWE-590/CWE-666 | `covered`: `stray-close`, `stray-free` |
| Double free | CWE-415, CERT MEM01-C/MEM30-C | `covered`: `double-free` |
| Use after free | CWE-416, CERT MEM30-C | `candidate`: read/write after `p101_free` with sanitizer/report lesson |
| Realloc ownership loss | CWE-401, CERT MEM31-C | `covered`: `realloc-leak` |
| Realloc failure handling | CERT MEM31-C/MEM35-C | `candidate`: preserve original pointer across failed grow |
| Exec descriptor inheritance | CWE-403/CWE-200, POSIX close-on-exec practice | `covered`: `exec-inherit` |
| Out-of-bounds write | CWE-787, CERT ARR30-C/STR31-C | `candidate`: fixed-size buffer write past end |
| Out-of-bounds read | CWE-125, CERT ARR30-C | `candidate`: read one past buffer end |
| Stack/heap buffer overflow | CWE-121/CWE-122/CWE-120 | `candidate`: unsafe copy or incorrect length |
| `sizeof(pointer)` used as array length | CWE-467, CERT ARR01-C | `covered`: `sizeof-pointer` |
| Ignoring read/write byte counts | CWE-252/CWE-200, CERT FIO | `covered`: `ignore-read-count` |
| Ignoring return values | CWE-252, JPL checked-return rule | `tool`: wrapper audit / static checks; add fixture if useful |
| Uninitialized read | CWE-457, CERT EXP33-C | `candidate`: branch/output based on uninitialized stack data |
| Null dereference | CWE-476, CERT EXP34-C | `candidate`: unchecked allocation/lookup result |
| Integer overflow in size calculation | CWE-190/CWE-680, CERT INT30-C/INT32-C | `candidate`: allocation size wrap |
| Signed/unsigned conversion mistake | CWE-195/CWE-681, CERT INT rules | `candidate`: negative length becomes large size |
| Truncation/narrowing | CWE-197, CERT INT rules | `candidate`: `size_t` to `int` command length |
| Missing input validation | CWE-20, CERT API00-C | `candidate`: unchecked option/range/path input |
| Path traversal | CWE-22 | `candidate`: output file path escapes intended directory |
| Command injection | CWE-78/CWE-77 | `candidate`: shell command built from user text; preferably commented/uncomment lab |
| Dangerous libc function | CWE-676, CERT STR/FIO rules | `tool`: wrapper audit / include doctor; use commented fixture |
| Format string misuse | CWE-134, CERT FIO30-C/FIO47-C | `candidate`: user-controlled format; may need commented fixture |
| Secret data left in reusable resource | CWE-226/CWE-200, CERT MEM03-C/MEM06-C | `candidate`: stale buffer or temp-file lesson |
| Predictable temporary file | CWE-377, CERT FIO21-C | `candidate`: insecure temp path construction |
| Resource exhaustion / missing limits | CWE-770, CERT MEM11-C | `candidate`: unbounded allocation/repeat count |
| Race / TOCTOU | CWE-367, CERT POS rules | `candidate`: check-then-open path demo |
| Data race / unsynchronized shared state | CWE-362, CERT CON rules | `candidate`: pthread counter fixture |
| Public API too broad / missing `static` | JPL narrow-scope rule, MISRA analyzability | `tool`: `p101-module-map` |
| Preprocessor misuse | CERT PRE, MISRA preprocessor rules | `tool`: include doctor / module map |
| Portability assumptions | CERT MSC/POS, MISRA implementation-defined behavior | `tool`: portability checks |

## Good-practice lab backlog

Not every playground lesson should begin with a bug. Some should teach what
good C looks like before the student sees the failure mode. These are positive
habits worth covering alongside the traps above.

| Practice | Canonical anchors | Playground status |
| --- | --- | --- |
| Structured logging with severity, event name, location, and outcome | OWASP Logging Cheat Sheet, NIST SSDF PW/RV practices | `candidate`: small logger wrapper and report correlation lesson |
| Security-event logging for validation failure, authz/authn-like decisions, unexpected control flow, and resource failures | OWASP Logging Cheat Sheet | `candidate`: validation-failure scenario with expected log event |
| Do not log secrets, tokens, passwords, raw PII, or sensitive file contents | OWASP Logging Cheat Sheet, CWE-532 | `candidate`: secret-redaction lesson |
| Neutralize untrusted data before writing it to logs | OWASP Logging Cheat Sheet, CWE-117 | `candidate`: log-injection lesson |
| Consistent error propagation using `p101_error` | CERT API04-C, ERR recommendations | `covered`: toolchain style, templates, wrapper examples |
| Every resource has one owner and one cleanup path | CERT MEM12-C, JPL narrow-control-flow guidance | `covered`: clean fixtures and resource-leak fixes |
| Prefer small functions with narrow scope and `static` internal helpers | MISRA analyzability, JPL narrow-scope guidance | `tool`: `p101-module-map` |
| Validate all external input at the boundary | CERT API00-C, CWE-20 | `candidate`: command-line/config validation lesson |
| Preserve the original value when acquisition/growth can fail | CERT MEM/realloc guidance | `candidate`: `realloc` failure lesson |
| Check return values or explicitly document intentional discard | CWE-252, JPL checked-return guidance | `tool`: static/tooling rule with commented fixture |
| Make tests cover both happy paths and failure paths | NIST SSDF, CERT conformance/testing guidance | `covered`: corpus, `p101-error-path-walk` |
| Add fuzz targets around parsers and boundary-heavy code | NIST SSDF verification practices | `covered`: project scripts; `candidate`: student parser lab |
| Keep a reproducible bug bundle for failures | NIST SSDF response/reproducibility practices | `covered`: `p101 bug-bundle` / `p101 check` |
| Prefer source-visible, plain-text diagnostics for teaching tools | p101 teaching principle, SSDF traceability | `covered`: TSV logs and markdown/HTML reports |

## Prioritization rule

Prefer labs that are:

1. common in student code;
2. visible through the p101 wrapper/event model;
3. explainable with one small broken function and one small fix;
4. mappable to a stable rule ID such as CERT, CWE, or MISRA.

If a mistake is better caught by the compiler or static analysis than by runtime
observation, keep the bad code commented in the lesson and make uncommenting it
part of the exercise. The committed repository should still build clean.

For positive-practice labs, prefer fixtures that let students compare a weak
implementation with a strong one. Logging is the model: a useful lab should show
both missing/unsafe logging and a fixed version with structured event names,
severity, safe fields, and no secret leakage.
