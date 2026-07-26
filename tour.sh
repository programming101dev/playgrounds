#!/usr/bin/env bash
# tour.sh — generate one self-contained playground artifact directory.
#
# The playground is meant to show the whole p101 toolchain in one place:
# strict build, tests, fuzzing, coverage, observation, resource tracking, call
# tracing, correlated reports, and error-path walking.
set -u
set -o pipefail

cd -- "$(dirname -- "${BASH_SOURCE[0]}")"

out_dir=""
fuzz_secs=5
fault_count=8
do_quality=1
do_coverage=1
do_fuzz=1

usage() {
  cat <<'USAGE'
Usage: ./tour.sh [options]

Create one report directory that demonstrates the p101 tooling pipeline.

Options:
  -o <dir>         Output directory.
                   Default: /tmp/p101-tool-playground-tour-<timestamp>-<pid>
  -t <seconds>    Fuzz smoke budget. Default: 5.
  -n <count>      Fault-injection cases for p101-error-path-walk. Default: 8.
  --skip-quality  Skip build/test/fuzz/coverage and only run runtime demos.
  --skip-coverage Skip coverage generation.
  --skip-fuzz     Skip fuzz smoke.
  -h, --help      Show this help.

Tool paths may be overridden with:
  P101_OBSERVE, P101_RESOURCE_TRACKER, P101_TRACE, P101_REPORT,
  P101_ERROR_PATH_WALK, P101_TOOL_PLAYGROUND
USAGE
}

while [ $# -gt 0 ]; do
  case "$1" in
    -h|--help) usage; exit 0 ;;
    -o) out_dir="${2:?}"; shift 2 ;;
    -t) fuzz_secs="${2:?}"; shift 2 ;;
    -n) fault_count="${2:?}"; shift 2 ;;
    --skip-quality) do_quality=0; shift ;;
    --skip-coverage) do_coverage=0; shift ;;
    --skip-fuzz) do_fuzz=0; shift ;;
    *) echo "Unknown option: $1" >&2; usage; exit 2 ;;
  esac
done

timestamp="$(date +%Y%m%d-%H%M%S)"
if [ -z "$out_dir" ]; then
  out_dir="/tmp/p101-tool-playground-tour-${timestamp}-$$"
fi

mkdir -p "$out_dir"
out_dir="$(cd "$out_dir" && pwd)"
log_dir="$out_dir/logs"
mkdir -p "$log_dir"

summary="$out_dir/summary.md"
: > "$summary"

pass_count=0
fail_count=0
skip_count=0

record() {
  status="$1"
  title="$2"
  detail="$3"

  case "$status" in
    PASS) pass_count=$((pass_count + 1)) ;;
    FAIL) fail_count=$((fail_count + 1)) ;;
    SKIP) skip_count=$((skip_count + 1)) ;;
  esac

  printf '| %s | %s | %s |\n' "$status" "$title" "$detail" >> "$summary"
}

relpath() {
  path="$1"
  case "$path" in
    "$out_dir"/*) printf '%s\n' "${path#"$out_dir"/}" ;;
    *) printf '%s\n' "$path" ;;
  esac
}

find_tool() {
  env_name="$1"
  shift

  eval "configured=\${$env_name:-}"
  if [ -n "$configured" ]; then
    if [ -x "$configured" ] || command -v "$configured" >/dev/null 2>&1; then
      printf '%s\n' "$configured"
      return 0
    fi
  fi

  for candidate in "$@"; do
    if [ -x "$candidate" ]; then
      printf '%s\n' "$candidate"
      return 0
    fi

    if command -v "$candidate" >/dev/null 2>&1; then
      command -v "$candidate"
      return 0
    fi
  done

  return 1
}

run_logged() {
  title="$1"
  log="$2"
  expected="$3"
  shift 3

  printf '\n==> %s\n' "$title"
  printf '$' > "$log"
  for arg in "$@"; do
    printf ' %s' "$arg" >> "$log"
  done
  printf '\n\n' >> "$log"

  set +e
  "$@" >> "$log" 2>&1
  rc=$?
  set +e

  for ok in $expected; do
    if [ "$rc" -eq "$ok" ]; then
      printf '    PASS (exit %s)\n' "$rc"
      record "PASS" "$title" "[log](./$(relpath "$log"))"
      return 0
    fi
  done

  printf '    FAIL (exit %s; see %s)\n' "$rc" "$log"
  record "FAIL" "$title" "[log](./$(relpath "$log"))"
  return 1
}

copy_coverage_report() {
  if [ -d coverage-clang ]; then
    rm -rf "$out_dir/coverage"
    cp -R coverage-clang "$out_dir/coverage"
  fi
}

missing_tools() {
  missing=""

  while [ $# -gt 1 ]; do
    name="$1"
    value="$2"
    shift 2

    if [ -z "$value" ]; then
      if [ -z "$missing" ]; then
        missing="$name"
      else
        missing="${missing}, ${name}"
      fi
    fi
  done

  printf '%s\n' "$missing"
}

reset_child_dir() {
  child="$1"

  case "$child" in
    "$out_dir"/*)
      rm -rf "$child"
      ;;
    *)
      printf 'Refusing to remove path outside output directory: %s\n' "$child" >&2
      return 1
      ;;
  esac
}

write_summary_header() {
  cat > "$summary" <<EOF
# p101-tool-playground tour

Generated: ${timestamp}

This directory is a one-command tour of the p101 tooling stack: strict checks,
unit tests, fuzzing, coverage, runtime observation, resource tracking, call
tracing, correlated reports, and fault-injected error-path walking.

## Results

| Status | Step | Artifact |
| --- | --- | --- |
EOF
}

append_summary_footer() {
  cat >> "$summary" <<EOF

## Totals

- PASS: ${pass_count}
- FAIL: ${fail_count}
- SKIP: ${skip_count}

## Runtime report directories
EOF

  if [ -d "$out_dir/observed-tour" ]; then
    printf '\n- Full clean tour: [observed-tour](./observed-tour/)\n' >> "$summary"
  fi

  if [ -d "$out_dir/observed-fd-leak" ]; then
    printf -- '- Descriptor leak: [observed-fd-leak](./observed-fd-leak/)\n' >> "$summary"
  fi

  if [ -d "$out_dir/observed-alloc-leak" ]; then
    printf -- '- Allocation leak: [observed-alloc-leak](./observed-alloc-leak/)\n' >> "$summary"
  fi

  if [ -d "$out_dir/observed-double-close" ]; then
    printf -- '- Double close: [observed-double-close](./observed-double-close/)\n' >> "$summary"
  fi

  if [ -d "$out_dir/fault-walk" ]; then
    printf -- '- Fault walk: [fault-walk](./fault-walk/)\n' >> "$summary"
  fi

  if [ -f "$out_dir/coverage/index.html" ]; then
    printf -- '- Coverage: [coverage/index.html](./coverage/index.html)\n' >> "$summary"
  fi

  cat >> "$summary" <<EOF

## Handy next reads
EOF

  if [ -f "$out_dir/observed-tour/summary.txt" ]; then
    printf '\n- [observed-tour/summary.txt](./observed-tour/summary.txt)\n' >> "$summary"
  fi

  if [ -f "$out_dir/observed-fd-leak/resource-report.txt" ]; then
    printf -- '- [observed-fd-leak/resource-report.txt](./observed-fd-leak/resource-report.txt)\n' >> "$summary"
  fi

  if [ -f "$out_dir/observed-alloc-leak/correlated-report.json" ]; then
    printf -- '- [observed-alloc-leak/correlated-report.json](./observed-alloc-leak/correlated-report.json)\n' >> "$summary"
  fi

  if [ -f "$log_dir/fault-walk.log" ]; then
    printf -- '- [fault-walk output](./logs/fault-walk.log)\n' >> "$summary"
  fi
}

write_summary_header

echo "p101-tool-playground tour output: $out_dir"

if [ "$do_quality" -eq 1 ]; then
  run_logged "configure clang build" "$log_dir/configure.log" "0" ./change-compiler.sh -c clang || true
  run_logged "strict build" "$log_dir/build.log" "0" ./build.sh -q || true
  run_logged "unit tests" "$log_dir/tests.log" "0" ./test.sh || true

  if [ "$do_fuzz" -eq 1 ] && [ -x ./fuzz.sh ] && ./fuzz.sh --can-fuzz >/dev/null 2>&1; then
    run_logged "fuzz smoke" "$log_dir/fuzz.log" "0" ./fuzz.sh -t "$fuzz_secs" || true
  elif [ "$do_fuzz" -eq 1 ]; then
    record "SKIP" "fuzz smoke" "no fuzzer-capable clang found"
  else
    record "SKIP" "fuzz smoke" "--skip-fuzz"
  fi

  if [ "$do_coverage" -eq 1 ] && command -v gcovr >/dev/null 2>&1; then
    run_logged "configure coverage build" "$log_dir/configure-coverage.log" "0" ./change-compiler.sh -c clang --coverage || true
    run_logged "coverage build" "$log_dir/build-coverage.log" "0" ./build.sh -q || true
    run_logged "coverage tests" "$log_dir/tests-coverage.log" "0" ./test.sh --coverage || true
    run_logged "coverage report" "$log_dir/coverage.log" "0" ./coverage-report.sh --no-open --min 1 -- -s tour || true
    copy_coverage_report
  elif [ "$do_coverage" -eq 1 ]; then
    record "SKIP" "coverage report" "gcovr not found"
  else
    record "SKIP" "coverage report" "--skip-coverage"
  fi
else
  record "SKIP" "quality pipeline" "--skip-quality"
fi

playground="$(find_tool P101_TOOL_PLAYGROUND ./build-clang/p101-tool-playground p101-tool-playground)"
observe="$(find_tool P101_OBSERVE ../p101-observe/build-clang/p101-observe p101-observe)"
tracker="$(find_tool P101_RESOURCE_TRACKER ../p101-resource-tracker/build-clang/p101-resource-tracker ../p101-resource-tracker/build-clang/resource-tracker p101-resource-tracker resource-tracker)"
trace="$(find_tool P101_TRACE ../p101-trace/build-clang/p101-trace p101-trace)"
report="$(find_tool P101_REPORT ../p101-report/build-clang/p101-report p101-report)"
walker="$(find_tool P101_ERROR_PATH_WALK ../p101-error-path-walk/build-clang/p101-error-path-walk ../p101-error-path-walk/build-clang/error-path-walk p101-error-path-walk error-path-walk)"

if [ -z "$playground" ]; then
  record "FAIL" "locate playground binary" "run ./build.sh first"
elif [ -z "$observe" ] || [ -z "$tracker" ] || [ -z "$trace" ] || [ -z "$report" ]; then
  record "SKIP" "observed runtime demos" "missing: $(missing_tools p101-observe "$observe" p101-resource-tracker "$tracker" p101-trace "$trace" p101-report "$report")"
else
  reset_child_dir "$out_dir/observed-tour"
  reset_child_dir "$out_dir/observed-fd-leak"
  reset_child_dir "$out_dir/observed-alloc-leak"
  reset_child_dir "$out_dir/observed-double-close"
  run_logged "observe full clean tour" "$log_dir/observe-tour.log" "0" "$observe" -o "$out_dir/observed-tour" -r "$tracker" -t "$trace" -p "$report" -- "$playground" -s tour -o "$out_dir/tour-output.txt" || true
  run_logged "observe fd leak" "$log_dir/observe-fd-leak.log" "0 1" "$observe" -o "$out_dir/observed-fd-leak" -r "$tracker" -t "$trace" -p "$report" -- "$playground" -s fd-leak -o "$out_dir/fd-leak-output.txt" || true
  run_logged "observe allocation leak" "$log_dir/observe-alloc-leak.log" "0 1" "$observe" -o "$out_dir/observed-alloc-leak" -r "$tracker" -t "$trace" -p "$report" -- "$playground" -s alloc-leak -o "$out_dir/alloc-leak-output.txt" || true
  run_logged "observe double close" "$log_dir/observe-double-close.log" "0 1" "$observe" -o "$out_dir/observed-double-close" -r "$tracker" -t "$trace" -p "$report" -- "$playground" -s double-close -o "$out_dir/double-close-output.txt" || true
fi

if [ -z "$playground" ] || [ -z "$walker" ] || [ -z "$tracker" ] || [ -z "$report" ]; then
  record "SKIP" "fault walk" "missing: $(missing_tools p101-tool-playground "$playground" p101-error-path-walk "$walker" p101-resource-tracker "$tracker" p101-report "$report")"
else
  reset_child_dir "$out_dir/fault-walk"
  mkdir -p "$out_dir/fault-walk"
  run_logged "fault walk" "$log_dir/fault-walk.log" "0 1" "$walker" -n "$fault_count" -l "$out_dir/fault-walk/fault" -r "$tracker" -p "$report" -- "$playground" -s fault-lab -o "$out_dir/fault-lab-output.txt" || true
fi

append_summary_footer

echo
echo "Tour complete: $out_dir"
echo "Summary: $summary"

if [ "$fail_count" -gt 0 ]; then
  exit 1
fi

exit 0
