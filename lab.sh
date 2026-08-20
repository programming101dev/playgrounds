#!/usr/bin/env bash
# lab.sh — render the p101-tool-playground lab book from a p101-test receipt.

set -euo pipefail
CDPATH='' cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

exec ./corpus/build-lab.py "$@"
