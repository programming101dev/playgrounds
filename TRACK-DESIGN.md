# p101 playground track design

The playground uses one repository with many small defect-and-repair tracks.

The old idea was to split the curriculum into several playground repositories.
The better shape is a single repo with explicit internal tracks:

- one build/test/fuzz/coverage/tooling setup;
- one lesson-fixture catalog, executed by `programs/p101-test`;
- one place for instructors and students to clone;
- small defect families so students are not handed a giant `systems` lab;
- no `misc` track.

Correct-use examples belong to `examples/<repo>`. A track may link to one, but
must not copy it or treat it as playground behavior coverage. The playground
owns the mistake, its expected diagnostic, the explanation of the violated
invariant, and the repair oracle.

The generated source of truth is [tracks/README.md](./tracks/README.md). Track
`00-p101-orientation` is the generated negative-control pre-track; it does not
own wrapper coverage or a correct-use implementation.

The first wrapper-family curated track is
[tracks/01-c-memory-runtime/TRACK.md](./tracks/01-c-memory-runtime/TRACK.md).
Use it as the pattern for later tracks: generated inventory in `README.md`, a
defect-and-repair sequence in `TRACK.md`, and a machine-readable map in
`track.json`. The local harness proves only inventory and linkage.

Wrapper assignment answers “where is this taught?” It does not prove that a
track exercises the wrapper. `track.json` therefore records either
`inventory-and-linkage`; executable defect evidence comes from the corpus
receipt. A track is curriculum-ready only after it has a focused intentional
defect, a stable diagnostic, a repair explanation, and a replayable oracle.
This prevents a program that merely prints function names from being counted
as a finished lesson.

## Rules

1. Every wrapper gets one primary track home.
2. Tracks may reference each other, but wrapper ownership should not overlap.
3. If a track starts to feel like a junk drawer, split it.
4. Never add a `misc` track. Name the concept instead.
5. Link to the owning examples repository when students need to see the valid
   shape; do not embed that shape here.
6. Never describe inventory or linkage as behavior coverage.

## Regeneration

The track map comes from the library function graph:

```sh
../scripts/generators/analyze-lib-function-graph.py
../scripts/generators/sync-playground-track-map.py
```
