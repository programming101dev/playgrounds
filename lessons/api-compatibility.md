# Review public API changes

Public interfaces are promises to other files and repositories. A change that
looks like cleanup inside one library can remove a function, move ownership to
another library, change the header consumers include, or quietly drop a
supported platform.

`p101 api-diff` compares two public-API manifests and reports:

- `P101-API-001`: a public declaration was removed;
- `P101-API-002`: a declaration moved to a different library;
- `P101-API-003`: a declaration moved to a different public header;
- `P101-API-004`: a declaration lost platform support.

## Workflow

1. Snapshot the API before the intended change.
2. Make the library-boundary change.
3. Snapshot the API again.
4. Run `p101 api-diff before.json after.json`.
5. For each finding, either restore compatibility or update every admitted
   consumer and record the intentional break.

The tool reports structural differences; it does not prove ABI compatibility
or semantic equivalence. Parameter layout, calling convention, behavior, and
binary compatibility still require compiler/ABI checks and consumer tests.
