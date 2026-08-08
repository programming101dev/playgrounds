#!/usr/bin/env bash
# lesson.sh — small, teachable p101-tool-playground lessons.
set -u
set -o pipefail

CDPATH='' cd -- "$(dirname -- "${BASH_SOURCE[0]}")" || exit 1

lesson="all"
out_dir=""

if [ "$#" -gt 0 ] && [ "${1#-}" = "$1" ]; then
  lesson="$1"
  shift
fi

usage() {
  cat <<'USAGE'
Usage: ./lesson.sh [all|wrappers|fd-leak|error-path|module-split] [-o <dir>]

Runs a focused teaching demo and writes a short summary. The canonical p101
runtime facade may be overridden with P101. Source tools may be overridden
with P101_WRAPPER_AUDIT, P101_MODULE_MAP, and P101_TOOL_PLAYGROUND.
USAGE
}

while [ "$#" -gt 0 ]; do
  case "$1" in
    -h|--help) usage; exit 0 ;;
    -o) out_dir="${2:?}"; shift 2 ;;
    *) echo "Unknown option: $1" >&2; usage; exit 2 ;;
  esac
done

if [ -z "$out_dir" ]; then
  out_dir="$(mktemp -d "/tmp/p101-tool-playground-lesson-$(date +%Y%m%d-%H%M%S).XXXXXX")"
fi

mkdir -p "$out_dir/logs"
out_dir="$(CDPATH='' cd -P "$out_dir" && pwd -P)"
summary="$out_dir/summary.md"
failures=0

find_tool() {
  env_name="$1"
  shift

  configured="$(printenv "$env_name" 2>/dev/null || true)"
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

run_step() {
  title="$1"
  log="$2"
  expected_rc="$3"
  shift 3

  printf '==> %s\n' "$title"

  # The command banner goes to a sibling file so that assertions against the
  # step log cannot be satisfied by the echoed command line itself.
  cmd_log="$log.cmd"
  printf '$' > "$cmd_log"
  for arg in "$@"; do
    printf ' %s' "$arg" >> "$cmd_log"
  done
  printf '\n' >> "$cmd_log"

  "$@" > "$log" 2>&1
  rc=$?

  for accepted_rc in $expected_rc; do
    if [ "$rc" -eq "$accepted_rc" ]; then
      printf '| PASS | %s | [log](./logs/%s) |\n' "$title" "$(basename "$log")" >> "$summary"
      return 0
    fi
  done

  printf '| FAIL | %s expected exit %s, got %s | [log](./logs/%s) |\n' "$title" "$expected_rc" "$rc" "$(basename "$log")" >> "$summary"
  return 1
}

assert_contains() {
  title="$1"
  file="$2"
  pattern="$3"

  if [ ! -f "$file" ]; then
    printf '| FAIL | %s | missing `%s` |\n' "$title" "$file" >> "$summary"
    return 1
  fi

  if grep -Eq "$pattern" "$file"; then
    printf '| PASS | %s | [%s](./%s) |\n' "$title" "$(basename "$file")" "${file#"$out_dir"/}" >> "$summary"
    return 0
  fi

  printf '| FAIL | %s missing `%s` | [%s](./%s) |\n' "$title" "$pattern" "$(basename "$file")" "${file#"$out_dir"/}" >> "$summary"
  return 1
}

assert_findings_counted() {
  title="$1"
  file="$2"

  if [ ! -f "$file" ]; then
    printf '| FAIL | %s | missing `%s` |\n' "$title" "$file" >> "$summary"
    return 1
  fi

  # Read the top-level summary count instead of grepping for any nested
  # "findings" key, which matches unrelated per-record structures.
  count="$(python3 -c 'import json, sys
data = json.load(open(sys.argv[1], encoding="utf-8"))
summary = data.get("summary")
value = summary.get("findings") if isinstance(summary, dict) else data.get("findings")
if isinstance(value, list):
    value = len(value)
print(value if isinstance(value, int) else -1)' "$file" 2>/dev/null || true)"

  if [ -n "$count" ] && [ "$count" -gt 0 ] 2>/dev/null; then
    printf '| PASS | %s | [%s](./%s) |\n' "$title" "$(basename "$file")" "${file#"$out_dir"/}" >> "$summary"
    return 0
  fi

  printf '| FAIL | %s reported no top-level findings | [%s](./%s) |\n' "$title" "$(basename "$file")" "${file#"$out_dir"/}" >> "$summary"
  return 1
}

assert_exists() {
  title="$1"
  path="$2"
  if [ -d "$path" ]; then
    if ! find "$path" -mindepth 1 -print -quit | grep -q .; then
      printf '| FAIL | %s | empty directory `%s` |\n' "$title" "$path" >> "$summary"
      return 1
    fi
  elif [ ! -e "$path" ]; then
    printf '| FAIL | %s | missing `%s` |\n' "$title" "$path" >> "$summary"
    return 1
  fi
  if [ -e "$path" ]; then
    printf '| PASS | %s | [%s](./%s) |\n' "$title" "$(basename "$path")" "${path#"$out_dir"/}" >> "$summary"
    return 0
  fi
  return 1
}

playground="$(find_tool P101_TOOL_PLAYGROUND ./build-clang-22/p101-tool-playground ./build-clang/p101-tool-playground ./build-gcc-16/p101-tool-playground p101-tool-playground || true)"
p101_dispatcher="$(find_tool P101 ../scripts/p101 p101 || true)"
wrapper_audit="$(find_tool P101_WRAPPER_AUDIT ../programs/p101-wrapper-audit/p101-wrapper-audit p101-wrapper-audit || true)"
module_map="$(find_tool P101_MODULE_MAP ../programs/p101-module-map/build-clang-22/p101-module-map ../programs/p101-module-map/build-clang/p101-module-map ../programs/p101-module-map/build-gcc-16/p101-module-map p101-module-map || true)"

cat > "$summary" <<EOF
# p101 playground lesson

Lesson: \`${lesson}\`

| Status | Step | Artifact |
| --- | --- | --- |
EOF

need_runtime_tools() {
  [ -n "$playground" ] && [ -n "$p101_dispatcher" ]
}

do_wrappers() {
  compile_db=""

  if [ -z "$wrapper_audit" ]; then
    printf '| FAIL | wrapper boundary | p101-wrapper-audit not found |\n' >> "$summary"
    failures=1
    return 1
  fi
  if [ -f compile_commands.json ]; then
    compile_db="$(pwd -P)/compile_commands.json"
  else
    for candidate in build-*/compile_commands.json; do
      if [ -f "$candidate" ]; then
        compile_db="$(pwd -P)/$candidate"
        break
      fi
    done
  fi
  if [ -z "$compile_db" ]; then
    printf '| FAIL | wrapper boundary | compile_commands.json not found; run ./change-compiler.sh and ./build.sh first |\n' >> "$summary"
    failures=1
    return 1
  fi
  run_step "wrapper boundary audit" "$out_dir/logs/wrappers.log" 0 "$wrapper_audit" --compile-db "$compile_db" src include || failures=1
  assert_contains "wrapper audit produced a summary" "$out_dir/logs/wrappers.log" "p101-wrapper-audit summary" || failures=1
  assert_contains "wrapper audit parsed every admitted file" "$out_dir/logs/wrappers.log" "parse_failures:[[:space:]]*0" || failures=1
  assert_contains "wrapper audit found no missed wrappers" "$out_dir/logs/wrappers.log" "missed_wrappers:[[:space:]]*0" || failures=1
}

do_fd_leak() {
  if ! need_runtime_tools; then
    printf '| FAIL | fd leak observation | runtime tools missing |\n' >> "$summary"
    failures=1
    return 1
  fi
  run_step "fd leak observation" "$out_dir/logs/fd-leak.log" 1 "$p101_dispatcher" run -o "$out_dir/fd-leak" -- "$playground" -s fd-leak -o "$out_dir/fd-leak-output.txt" || failures=1
  assert_contains "fd leak report uses the current schema" "$out_dir/fd-leak/analysis/resource-report.json" "\"schema\"[[:space:]]*:[[:space:]]*\"p101-resource-policy-findings-v1\"" || failures=1
  assert_findings_counted "fd leak is counted" "$out_dir/fd-leak/analysis/resource-report.json" || failures=1
}

do_error_path() {
  if ! need_runtime_tools; then
    printf '| FAIL | error path walk | runtime tools missing |\n' >> "$summary"
    failures=1
    return 1
  fi
  mkdir -p "$out_dir/fault-walk"
  run_step "error path walk" "$out_dir/logs/error-path.log" "0 1" "$p101_dispatcher" walk -n 8 -l "$out_dir/fault-walk/case" -- "$playground" -s fault-lab -o "$out_dir/fault-output.txt" || failures=1
  assert_contains "fault walk produced evidence" "$out_dir/logs/error-path.log" "fault|case|finding|leak" || failures=1
  assert_exists "fault walk produced case artifacts" "$out_dir/fault-walk" || failures=1
}

do_module_split() {
  if [ -z "$module_map" ]; then
    printf '| FAIL | module split map | p101-module-map not found |\n' >> "$summary"
    failures=1
    return 1
  fi
  run_step "module split map" "$out_dir/logs/module-map.log" "0 1" "$module_map" -o "$out_dir/module-map.md" src include || failures=1
  assert_contains "module map has structure" "$out_dir/module-map.md" "Modules|Teaching notes" || failures=1
}

case "$lesson" in
  all)
    do_wrappers
    do_fd_leak
    do_error_path
    do_module_split
    ;;
  wrappers) do_wrappers ;;
  fd-leak) do_fd_leak ;;
  error-path) do_error_path ;;
  module-split) do_module_split ;;
  *) echo "Unknown lesson: $lesson" >&2; usage; exit 2 ;;
esac

cat >> "$summary" <<EOF

## Suggested read order

1. Open this summary.
2. Open the linked log for the selected lesson.
3. For runtime lessons, open the generated observe directory and compare
   \`summary.txt\`, \`resource-report.txt\`, \`trace-summary.txt\`, and
   \`resource-lifetimes.md\`.
EOF

echo "p101 playground lesson output: $out_dir"
echo "Summary: $summary"

if [ "$failures" -ne 0 ]; then
  echo "p101 playground lesson failed self-checks" >&2
  exit 1
fi
