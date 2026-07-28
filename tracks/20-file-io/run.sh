#!/usr/bin/env bash
# Build and run the standalone file-io playground track.

set -euo pipefail

track_dir="$(CDPATH= cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd -- "${track_dir}"

compiler="${CC:-clang}"
reconfigure=0
build_args=(-q)
program_args=()

usage() {
    echo "usage: $0 [-c compiler] [--reconfigure] [--] [track-args...]" >&2
}

while [[ $# -gt 0 ]]; do
    case "$1" in
        -h|--help) usage; exit 0 ;;
        -c|--compiler) compiler="${2:?}"; shift 2 ;;
        --reconfigure) reconfigure=1; shift ;;
        --verbose-build) build_args=(); shift ;;
        --) shift; program_args=("$@"); break ;;
        *) program_args+=("$1"); shift ;;
    esac
done

if [[ "${reconfigure}" -eq 1 || ! -f .last-build-dir ]]; then
    ./change-compiler.sh -c "${compiler}"
fi

./build.sh "${build_args[@]}"

build_dir="$(cat .last-build-dir)"
exec "${track_dir}/${build_dir}/p101-track-file-io" ${program_args[@]+"${program_args[@]}"}
