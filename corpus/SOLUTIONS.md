# p101-tool-playground instructor solution notes

The lab corpus is intentionally committed in its broken state. The instructor
answer key is the fixed signal for each lab, not a giant patch file:

- for resource-tracker labs, the finding ID listed in `expected.json` should
  disappear;
- for error-path labs, the injected-failure walk should become clean;
- for logic/output labs, the `fixed_output_contains` and
  `fixed_output_not_contains` fields define the target output.

Generate the current answer-key view with:

```sh
./lab.sh --strict-corpus
```

Open `lab.md` or `index.html` and look for “Instructor answer key signal” on
each lab card. This keeps the key synchronized with the checked corpus instead
of letting separate prose drift away from the executable oracle.

If you want a patch-style solution set for a course offering, create it as a
course-specific branch. The main branch should remain broken so students can run
the lab series and see progress from `OPEN` to `FIXED`.
