#!/usr/bin/env bash
# corpus.sh — run the playground lesson corpus through p101 check.

set -euo pipefail
CDPATH= cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

exec ./corpus/run-corpus.py "$@"
