#!/usr/bin/env bash
# Canonical standalone playground-track runner.
#
# scripts/distribution/copy-playground-track-scripts.sh materializes this file as run.sh in
# every track so copied tracks remain self-contained without allowing 41
# private runner implementations to drift apart.
set -euo pipefail

track_dir="$(CDPATH='' cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
track_name="${track_dir##*/}"
case "$track_name" in
    [0-9][0-9]-*) program_name="p101-track-${track_name#??-}" ;;
    *)
        printf 'Error: track directory must begin with NN-: %s\n' "$track_name" >&2
        exit 2 ;;
esac
cd -- "$track_dir"

compiler="${CC:-clang}"
reconfigure=0
build_args=(-q)
program_args=()

usage() {
    printf 'usage: %s [-c compiler] [--reconfigure] [--verbose-build] [--] [track-args...]\n' "$0" >&2
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help) usage; exit 0 ;;
        -c|--compiler)
            [[ $# -ge 2 ]] || { usage; exit 2; }
            compiler="$2"
            shift 2 ;;
        --reconfigure) reconfigure=1; shift ;;
        --verbose-build) build_args=(); shift ;;
        --) shift; program_args=("$@"); break ;;
        *) program_args+=("$1"); shift ;;
    esac
done

if [[ "$reconfigure" -eq 1 || ! -f .last-build-dir ]]; then
    ./change-compiler.sh -c "$compiler"
fi

./build.sh ${build_args[@]+"${build_args[@]}"}

IFS= read -r build_dir < .last-build-dir || {
    echo "Error: could not read .last-build-dir." >&2
    exit 1
}
[[ -n "$build_dir" ]] || {
    echo "Error: .last-build-dir is empty." >&2
    exit 1
}
program="$track_dir/$build_dir/$program_name"
[[ -x "$program" ]] || {
    printf 'Error: track executable was not produced: %s\n' "$program" >&2
    exit 1
}
exec "$program" ${program_args[@]+"${program_args[@]}"}
