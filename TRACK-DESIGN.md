# p101 playground track design

The playground uses one repository with many small tracks.

The old idea was to split the curriculum into several playground repositories.
The better shape is a single repo with explicit internal tracks:

- one build/test/fuzz/coverage/tooling setup;
- one corpus runner;
- one place for instructors and students to clone;
- small lesson families so students are not handed a giant `systems` lab;
- no `misc` track.

The generated source of truth is [tracks/README.md](./tracks/README.md).

## Rules

1. Every wrapper gets one primary track home.
2. Tracks may reference each other, but wrapper ownership should not overlap.
3. If a track starts to feel like a junk drawer, split it.
4. Never add a `misc` track. Name the concept instead.
5. Good examples come first; broken labs come after students have seen the valid
   shape.

## Regeneration

The track map comes from the library function graph:

```sh
../../scripts/analyze-lib-function-graph.py
./sync-track-map.py
```

