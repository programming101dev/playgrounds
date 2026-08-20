# Playground scope and removal record

## Admitted responsibility

The playground owns only defect-and-repair explanations for diagnostics that a
p101 tool can emit. `lessons/manifest.json` is the source of truth. Every
registered finding ID must appear exactly once as a level-two example heading
in its declared lesson, and no unregistered example heading is permitted.

The current admitted set is 111 diagnostic examples in 14 lesson pages.

## Supporting work that remains outside the playground

The following work is necessary, but it is deliberately owned elsewhere:

| Responsibility | Owner |
| --- | --- |
| Detect source, runtime, policy, mutation, API, and test-evidence defects | The emitting library or program |
| Execute native broken/fixed acceptance suites | `p101-audit`, `p101-inspect`, `p101-test`, and their libraries |
| Generate the native lesson lookup table | `scripts` and `lib_tool_support` |
| Schedule workspace acceptance and retain receipts | `scripts` |
| Demonstrate correct API use | `examples/<repo>` repositories |
| Organize stories, courses, weeks, quizzes, assignments, and projects | Course repositories, not this repository |

These responsibilities are recorded here so future changes do not quietly
turn support infrastructure into additional playground curriculum.

## Material removed from the playground scope

The diagnostic-only boundary retires the previous wrapper-course track map,
orientation course, lab book, submission/reset workflow, broad security and C
mistake corpus, executable tour program, fuzz harness, and causal-model demo
expectations. Those artifacts described work beyond the registered p101
diagnostics and duplicated responsibilities now owned by tools, scripts, and
example repositories.

Removal does not claim those topics are unimportant. It records only that they
are not playground examples until a p101 tool emits a stable diagnostic for
them and the manifest admits that diagnostic.

## Integration changes made with this boundary

| Change | Reason |
| --- | --- |
| Removed the playground tour, track-map generator, and playground graph nodes from `scripts` | They orchestrated course material outside the registered diagnostics. |
| Removed the orphaned playground corpus engine from `p101-test` | Owning-tool native suites already provide executable detection evidence; a second corpus had no admitted fixtures after this boundary. |
| Kept the semantic library function-graph generator but removed its playground-track recommendation policy | The call/domain graph is useful boundary evidence, but it must not design playground curriculum. Stale generated graph reports were removed until the native facts tool is available to regenerate them. |
| Added stable per-ID anchors to lesson pages and generated native routes | A diagnostic now links to its exact example rather than only to a shared lesson file. |

## Change rule

A new playground example is admitted only when the same change:

1. adds a stable diagnostic ID to an emitting tool;
2. adds native broken and repaired acceptance evidence to the owning tool;
3. maps the ID in `lessons/manifest.json`;
4. adds exactly one matching lesson section; and
5. regenerates and verifies the shared native lesson catalog.
