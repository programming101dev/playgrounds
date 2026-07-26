#!/usr/bin/env bash
# submit-labs.sh — student-facing pre-submission checks for the playground labs.

set -euo pipefail
cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

out="${1:-/tmp/p101-tool-playground-submit}"

if [[ -e "$out" ]]; then
  echo "submit-labs: output path already exists: $out" >&2
  echo "Choose another path, or remove the old directory." >&2
  exit 2
fi

echo "==> build"
./build.sh -q

echo "==> unit tests"
./test.sh

echo "==> lab progress"
./lab.sh -o "$out"

echo "Submission check output: $out"
echo "Open: $out/index.html"
