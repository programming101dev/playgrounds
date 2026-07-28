#!/usr/bin/env bash
# lab.sh — build the self-contained p101-tool-playground lab book.

set -euo pipefail
CDPATH='' cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

exec ./corpus/build-lab.py "$@"
