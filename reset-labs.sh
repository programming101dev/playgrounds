#!/usr/bin/env bash
# reset-labs.sh — restore the committed p101-tool-playground lab fixtures.

set -euo pipefail
CDPATH='' cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

usage() {
  cat <<'USAGE'
Usage: ./reset-labs.sh --show
       ./reset-labs.sh --yes

This helper is for students who are experimenting inside the playground labs.
It restores the committed lab source/corpus files. Use --show first if you want
to see exactly what would be reset.
USAGE
}

paths=(
  README.md
  include/arguments.h
  src/playground.c
  src/scenario.c
  test/test_arguments.c
  corpus/CANONICAL-SOURCES.md
  corpus/build-lab.py
  corpus/cases
)

case "${1:-}" in
  --show)
    echo "Would run:"
    printf '  git restore --'
    printf ' %q' "${paths[@]}"
    printf '\n'
    ;;
  --yes)
    git restore -- "${paths[@]}"
    echo "Restored committed lab fixtures."
    ;;
  -h|--help|"")
    usage
    ;;
  *)
    usage >&2
    exit 2
    ;;
esac
