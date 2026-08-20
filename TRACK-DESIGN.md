# p101 playground track design

The playground uses one repository with many small tracks.

The old idea was to split the curriculum into several playground repositories.
The better shape is a single repo with explicit internal tracks:

- one build/test/fuzz/coverage/tooling setup;
- one corpus runner;
- one place for instructors and students to clone;
- small lesson families so students are not handed a giant `systems` lab;
- no `misc` track.

The generated source of truth is [tracks/README.md](./tracks/README.md). Track
`00-p101-orientation` is the hand-authored pre-track; it is not generated from
the wrapper graph and does not own wrapper coverage.

The first wrapper-family curated track is
[tracks/01-c-memory-runtime/TRACK.md](./tracks/01-c-memory-runtime/TRACK.md).
Use it as the pattern for later tracks: generated inventory in `README.md`,
curated lesson sequence in `TRACK.md`, machine-readable case map in
`track.json`, and a local `run.sh` for the focused smoke run.

Wrapper assignment answers “where is this taught?” It does not prove that a
track exercises the wrapper. `track.json` therefore records either
`executable-behavior` or `inventory-and-linkage`. A track is curriculum-ready
only after it starts with a checked valid behavior example, follows it with a
focused failure/trap, and retains a regression assertion. This prevents a
program that merely prints function names from being counted as a finished
lesson.

## Rules

1. Every wrapper gets one primary track home.
2. Tracks may reference each other, but wrapper ownership should not overlap.
3. If a track starts to feel like a junk drawer, split it.
4. Never add a `misc` track. Name the concept instead.
5. Good examples come first; broken labs come after students have seen the valid
   shape.
6. Never describe inventory or linkage as behavior coverage.

## Regeneration

The track map comes from the library function graph:

```sh
../scripts/generators/analyze-lib-function-graph.py
../scripts/generators/sync-playground-track-map.py
```
