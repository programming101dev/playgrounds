#!/usr/bin/env bash
# submit-labs.sh — student-facing pre-submission checks for the playground labs.

set -euo pipefail
CDPATH='' cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

out="/tmp/p101-tool-playground-submit"
corpus_out=""
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

corpus_out="${out}.corpus"

if [[ -e "$out" || -e "$corpus_out" ]]; then
  echo "submit-labs: output path already exists: $out or $corpus_out" >&2
  echo "Choose another path, or remove the old directory." >&2
  exit 2
fi

echo "==> build"
cmake -S . -B build-submit -DP101_BUILD_LEVEL=2
cmake --build build-submit

echo "==> lab progress"
set +e
../programs/p101-test/test-corpus --strict --keep-going -o "$corpus_out"
corpus_status=$?
set -e
if ((corpus_status > 1)) || [[ ! -f "$corpus_out/receipt.json" ]]; then
  echo "submit-labs: corpus execution did not produce a complete receipt" >&2
  exit 2
fi
./lab.sh "${lab_args[@]}" --receipt "$corpus_out/receipt.json" --require-all-fixed -o "$out"

echo "Submission check output: $out"
echo "Open: $out/index.html"
