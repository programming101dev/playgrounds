#!/usr/bin/env bash
# Run one playground track through its CMake project.
set -euo pipefail

playground_root="$(CDPATH='' cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
track_id=""

compiler="${CC:-clang}"
reconfigure=0
build_args=()
program_args=()

usage() {
    printf 'usage: %s <track-id> [-c compiler] [--reconfigure] [--verbose-build] [--] [track-args...]\n' "$0" >&2
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help) usage; exit 0 ;;
        -c|--compiler)
            [[ $# -ge 2 ]] || { usage; exit 2; }
            compiler="$2"
            shift 2 ;;
        --reconfigure) reconfigure=1; shift ;;
        --verbose-build) build_args=(--verbose); shift ;;
        --) shift; program_args=("$@"); break ;;
        *)
            if [[ -z "$track_id" ]]; then
                track_id="$1"
            else
                program_args+=("$1")
            fi
            shift ;;
    esac
done

[[ -n "$track_id" ]] || { usage; exit 2; }
matches=("$playground_root"/tracks/[0-9][0-9]-"$track_id")
[[ "${#matches[@]}" -eq 1 && -d "${matches[0]}" ]] || {
    printf 'Error: unknown or ambiguous playground track: %s\n' "$track_id" >&2
    exit 2
}
track_dir="${matches[0]}"
track_name="${track_dir##*/}"
program_name="p101-track-${track_name#??-}"
build_dir="$track_dir/build-$(basename "$compiler")"

if [[ "$reconfigure" -eq 1 || ! -f "$build_dir/CMakeCache.txt" ]]; then
    rm -rf -- "$build_dir"
    cmake -S "$track_dir" -B "$build_dir" \
        -DCMAKE_C_COMPILER="$compiler" -DP101_BUILD_LEVEL=1 \
        -DP101_USE_PROBED_FLAGS=OFF
fi

cmake --build "$build_dir" ${build_args[@]+"${build_args[@]}"}

program="$build_dir/$program_name"
[[ -x "$program" ]] || {
    printf 'Error: track executable was not produced: %s\n' "$program" >&2
    exit 1
}
exec "$program" ${program_args[@]+"${program_args[@]}"}
