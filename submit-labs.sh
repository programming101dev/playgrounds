#!/usr/bin/env bash
# submit-labs.sh — student-facing pre-submission checks for the playground labs.

set -euo pipefail
CDPATH='' cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

out="/tmp/p101-tool-playground-submit"
lab_args=()

while (($#)); do
  case "$1" in
    -o|--output)
      if (($# < 2)); then
        echo "submit-labs: $1 requires a path" >&2
        exit 2
      fi
      out="$2"
      shift 2
      ;;
    --track)
      if (($# < 2)); then
        echo "submit-labs: --track requires c, systems, or network" >&2
        exit 2
      fi
      lab_args+=(--track "$2")
      shift 2
      ;;
    -h|--help)
      echo "Usage: ./submit-labs.sh [-o OUTPUT] [--track c|systems|network]"
      exit 0
      ;;
    *)
      echo "submit-labs: unknown argument: $1" >&2
      exit 2
      ;;
  esac
done

if [[ -e "$out" ]]; then
  echo "submit-labs: output path already exists: $out" >&2
  echo "Choose another path, or remove the old directory." >&2
  exit 2
fi

echo "==> build"
cmake -S . -B build-submit -DP101_BUILD_LEVEL=2
cmake --build build-submit

echo "==> lab progress"
./lab.sh "${lab_args[@]}" --require-all-fixed -o "$out"

echo "Submission check output: $out"
echo "Open: $out/index.html"
