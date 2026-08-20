# p101 diagnostic playground

This repository explains the programming defects emitted by the p101 tools.
Its admitted teaching surface is exactly the diagnostic IDs registered in
[`lessons/manifest.json`](./lessons/manifest.json).

At present that means:

- 76 defect-and-repair examples;
- 11 shared concept lessons;
- one stable lesson route for every registered diagnostic;
- no course, week, assignment, project, quiz, or grading structure;
- no unrelated collection of general C or security mistakes.

Each diagnostic has one `## P101-...` section in its owning lesson. The section
shows the smallest useful broken signal and the repair that should remove the
finding. Related diagnostics share a lesson page when the repair requires the
same underlying concept.

Correct programs do not live here. They belong in the matching
`examples/<repo>` repository. Executable detection tests do not live here
either; they remain with `p101-audit`, `p101-inspect`, `p101-test`, or the
shared analysis library that emits the diagnostic.

Tool diagnostics resolve the catalog through `lib_tool_support`. Human output
includes a `learn more` note and JSON output includes the same lesson ID, path,
and URL.

## Verify the scope

From `scripts`:

```sh
./runtime/p101_lessons.py check
./generators/generate-tool-lesson-catalog.sh --check
```

The checks require the registered IDs, lesson example headings, generated C
catalog, and owning-tool acceptance evidence to agree exactly.

See [`SCOPE.md`](./SCOPE.md) for the admitted responsibilities and the removal
record for material that used to make this repository behave like a course.
