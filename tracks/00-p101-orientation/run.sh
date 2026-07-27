#!/usr/bin/env bash
# Run the p101 orientation track.

set -euo pipefail

track_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
repo_root="$(cd -- "${track_dir}/../.." && pwd)"
output_dir="/tmp/p101-track-orientation"

usage() {
    echo "usage: $0 [-o output-dir] [--full]" >&2
}

if [[ "${1:-}" == "-o" || "${1:-}" == "--output" ]]; then
    if [[ $# -lt 2 ]]; then
        usage
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
    usage
    exit 2
fi

if [[ -e "${output_dir}" ]]; then
    echo "p101 orientation: output path already exists: ${output_dir}" >&2
    exit 2
fi

mkdir -p "${output_dir}/logs"
summary="${output_dir}/summary.md"
failures=0

run_step() {
    local title="$1"
    local log="$2"
    local expected_rc="$3"
    shift 3

    printf '==> %s\n' "${title}"
    printf '$' > "${log}"
    for arg in "$@"; do
        printf ' %s' "${arg}" >> "${log}"
    done
    printf '\n\n' >> "${log}"

    set +e
    "$@" >> "${log}" 2>&1
    local rc=$?
    set -e

    if [[ "${rc}" -eq "${expected_rc}" ]]; then
        printf '    PASS\n'
        printf '| PASS | %s | [log](./logs/%s) |\n' "${title}" "$(basename "${log}")" >> "${summary}"
        return 0
    fi

    printf '    FAIL (exit %s; see %s)\n' "${rc}" "${log}"
    printf '| FAIL | %s expected exit %s, got %s | [log](./logs/%s) |\n' "${title}" "${expected_rc}" "${rc}" "$(basename "${log}")" >> "${summary}"
    failures=1
    return 0
}

cd -- "${repo_root}"

cat > "${summary}" <<EOF
# p101 orientation track

Output: \`${output_dir}\`

| Status | Step | Artifact |
| --- | --- | --- |
EOF

run_step "orientation corpus cases" "${output_dir}/logs/corpus.log" 0 \
    ./corpus.sh \
    -o "${output_dir}/corpus" \
    --keep-going \
    "${skip_flags[@]}" \
    --case orientation \
    --case clean

run_step "tool mini-lessons" "${output_dir}/logs/tools.log" 0 \
    ./lesson.sh all \
    -o "${output_dir}/tools"

cat >> "${summary}" <<EOF

## Open next

1. [corpus/summary.md](./corpus/summary.md)
2. [tools/summary.md](./tools/summary.md)
3. [Track 00 lesson](${repo_root}/tracks/00-p101-orientation/TRACK.md)

## Source reading order

1. \`src/main.c\`
2. \`src/playground.c\` / \`run_orientation_demo\`
3. \`corpus/cases/orientation/lesson.md\`
EOF

echo "p101 orientation output: ${output_dir}"
echo "Summary: ${summary}"

if [[ "${failures}" -ne 0 ]]; then
    echo "p101 orientation failed" >&2
    exit 1
fi
