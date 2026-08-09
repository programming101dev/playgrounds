#!/usr/bin/env bash
# corpus.sh — run the playground lesson corpus through the student workflow.

set -euo pipefail
CDPATH='' cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

exec ./corpus/run-corpus.py "$@"
