#!/usr/bin/env bash
# Run the curated c-memory-runtime track cases.

set -euo pipefail

track_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "${track_dir}/../.." && pwd)"
output_dir="/tmp/p101-track-c-memory-runtime"

if [[ "${1:-}" == "-o" || "${1:-}" == "--output" ]]; then
    if [[ $# -lt 2 ]]; then
        echo "usage: $0 [-o output-dir] [--full]" >&2
        exit 2
    fi
    output_dir="$2"
    shift 2
fi

skip_flags=(--skip-html --skip-bundle)
if [[ "${1:-}" == "--full" ]]; then
    skip_flags=()
    shift
fi

if [[ $# -ne 0 ]]; then
    echo "usage: $0 [-o output-dir] [--full]" >&2
    exit 2
fi

cd -- "${repo_root}"

exec ./corpus.sh \
    -o "${output_dir}" \
    --keep-going \
    "${skip_flags[@]}" \
    --case clean \
    --case alloc-leak \
    --case early-return-alloc-leak \
    --case realloc-leak \
    --case realloc-failure \
    --case double-free \
    --case stray-free \
    --case use-after-free \
    --case resource-exhaustion
