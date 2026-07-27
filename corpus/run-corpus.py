#!/usr/bin/env python3
"""Run the p101-tool-playground lesson corpus through p101 check."""

from __future__ import annotations

import argparse
import json
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any


@dataclass(frozen=True)
class CaseResult:
    name: str
    status: str
    expected_exit: int
    actual_exit: int
    report_dir: Path
    message: str


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Run p101-tool-playground corpus cases through p101 check.")
    parser.add_argument("-o", "--output", type=Path, help="Output directory. Default: /tmp/p101-tool-playground-corpus-<pid>")
    parser.add_argument("--case", action="append", dest="cases", help="Run only this case name; may be repeated.")
    parser.add_argument("--track", choices=("c", "systems", "network"), help="Run only cases assigned to this playground track.")
    parser.add_argument("--quick", action="store_true", help="Run only the clean and fd-leak cases.")
    parser.add_argument("--keep-going", action="store_true", help="Continue after a failed case.")
    parser.add_argument("--p101", type=Path, help="Path to the p101 dispatcher.")
    parser.add_argument("--playground", type=Path, help="Path to p101-tool-playground executable.")
    parser.add_argument("--skip-html", action="store_true", help="Pass --skip-html to p101 check.")
    parser.add_argument("--skip-bundle", action="store_true", help="Pass --skip-bundle to p101 check.")
    return parser.parse_args(argv)


def read_json(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError(f"{path} did not contain a JSON object")
    return data


def find_executable(candidates: list[Path | str]) -> Path:
    for candidate in candidates:
        path = candidate if isinstance(candidate, Path) else Path(candidate)
        if path.is_file() and path.stat().st_mode & 0o111:
            return path.resolve()
        resolved = shutil.which(str(candidate))
        if resolved is not None:
            return Path(resolved).resolve()
    raise FileNotFoundError("none of the candidate executables were found: " + ", ".join(str(c) for c in candidates))


def current_playground_candidates(root: Path) -> list[Path | str]:
    candidates: list[Path | str] = []
    last_build = root / ".last-build-dir"
    try:
        build_dir = last_build.read_text(encoding="utf-8").strip()
    except OSError:
        build_dir = ""
    if build_dir:
        candidates.append(root / build_dir / "p101-tool-playground")
    candidates.extend([root / "build-clang/p101-tool-playground", root / "build-clang-22/p101-tool-playground", root / "build-gcc-16/p101-tool-playground", "p101-tool-playground"])
    return candidates


def case_tracks(case: dict[str, Any]) -> set[str]:
    tracks = case.get("tracks", [])
    if isinstance(tracks, list):
        return {str(item) for item in tracks}
    if isinstance(tracks, str):
        return {tracks}
    return set()


def load_cases(cases_dir: Path, selected: set[str] | None, track: str | None) -> list[dict[str, Any]]:
    cases: list[dict[str, Any]] = []
    for expected_path in sorted(cases_dir.glob("*/expected.json")):
        case = read_json(expected_path)
        case["case_dir"] = str(expected_path.parent)
        name = str(case.get("name"))
        if selected is not None and name not in selected:
            continue
        if track is not None and track not in case_tracks(case):
            continue
        if selected is None or name in selected:
            cases.append(case)
    cases.sort(key=lambda item: (int(item.get("lab_order", 1000)), str(item.get("name", ""))))
    return cases


def read_correlated_ids(report_dir: Path) -> set[str]:
    path = report_dir / "doctor" / "observe" / "correlated-report.json"
    try:
        data = read_json(path)
    except (OSError, json.JSONDecodeError, ValueError):
        return set()
    findings = data.get("findings")
    if not isinstance(findings, list):
        return set()
    ids: set[str] = set()
    for finding in findings:
        if isinstance(finding, dict) and isinstance(finding.get("id"), str):
            ids.add(finding["id"])
    return ids


def correlated_file_ids(path: Path) -> set[str]:
    try:
        data = read_json(path)
    except (OSError, json.JSONDecodeError, ValueError):
        return set()
    findings = data.get("findings")
    if not isinstance(findings, list):
        return set()
    ids: set[str] = set()
    for finding in findings:
        if isinstance(finding, dict) and isinstance(finding.get("id"), str):
            ids.add(finding["id"])
    return ids


def fault_walk_has_findings(report_dir: Path) -> bool:
    fault_dir = report_dir / "doctor" / "fault-walk"
    for path in fault_dir.glob("*.observe/correlated-report.json"):
        if correlated_file_ids(path):
            return True
        try:
            data = read_json(path)
        except (OSError, json.JSONDecodeError, ValueError):
            continue
        summary = data.get("summary")
        if isinstance(summary, dict) and isinstance(summary.get("findings"), int) and summary["findings"] > 0:
            return True
    for path in fault_dir.glob("*.observe/resource-report.json"):
        try:
            data = read_json(path)
        except (OSError, json.JSONDecodeError, ValueError):
            continue
        for key in ("fd_leaks", "allocation_leaks", "bad_releases", "exec_inheritances"):
            value = data.get(key)
            if isinstance(value, int) and value > 0:
                return True
        findings = data.get("findings")
        if isinstance(findings, list) and findings:
            return True
    for path in fault_dir.glob("*.observe/summary.txt"):
        text = path.read_text(encoding="utf-8", errors="replace")
        if "fd_leaks=0 allocation_leaks=0 bad_releases=0" not in text:
            return True
    return False


def doctor_fault_walk_status(report_dir: Path) -> int | None:
    try:
        data = read_json(report_dir / "doctor" / "doctor.json")
    except (OSError, json.JSONDecodeError, ValueError):
        return None
    statuses = data.get("statuses")
    if not isinstance(statuses, dict):
        return None
    value = statuses.get("p101_error_path_walk")
    return value if isinstance(value, int) else None


def run_case(root: Path, p101: Path, playground: Path, out_dir: Path, case: dict[str, Any], skip_html: bool, skip_bundle: bool) -> CaseResult:
    name = str(case["name"])
    scenario = str(case["scenario"])
    case_out = out_dir / name
    output_file = case_out / "playground-output.txt"
    log_path = out_dir / "logs" / f"{name}.log"
    expected_exit = int(case["expected_exit"])
    fault_count = int(case.get("fault_count", 1))
    expected_findings = {str(item) for item in case.get("expected_findings", [])}
    expected_error_path_findings = bool(case.get("expected_error_path_findings", False))
    expected_output_size = case.get("expected_output_size")
    expected_output_contains = [str(item) for item in case.get("expected_output_contains", [])]
    expected_output_missing = [str(item) for item in case.get("expected_output_missing", [])]

    command = [
        str(p101),
        "check",
        "--skip-quality",
        "-p",
        str(root),
        "-s",
        "src",
        "-n",
        str(fault_count),
        "-o",
        str(case_out),
    ]
    if skip_html:
        command.append("--skip-html")
    if skip_bundle:
        command.append("--skip-bundle")
    command.extend(["--", str(playground), "-s", scenario, "-o", str(output_file)])

    with log_path.open("w", encoding="utf-8") as log:
        log.write("$ " + " ".join(command) + "\n\n")
        completed = subprocess.run(command, cwd=root, stdout=log, stderr=subprocess.STDOUT, check=False)

    problems: list[str] = []
    if completed.returncode != expected_exit:
        problems.append(f"expected exit {expected_exit}, got {completed.returncode}")

    actual_ids = read_correlated_ids(case_out)
    missing = sorted(expected_findings - actual_ids)
    if missing:
        problems.append("missing finding IDs: " + ", ".join(missing))

    if expected_error_path_findings and doctor_fault_walk_status(case_out) != 1 and not fault_walk_has_findings(case_out):
        problems.append("expected error-path findings, but fault walk looked clean")

    if expected_output_size is not None:
        try:
            actual_output_size = output_file.stat().st_size
        except OSError:
            problems.append(f"expected output size {expected_output_size}, but output file was missing")
        else:
            if actual_output_size != int(expected_output_size):
                problems.append(f"expected output size {expected_output_size}, got {actual_output_size}")

    if expected_output_contains or expected_output_missing:
        try:
            output_text = output_file.read_text(encoding="utf-8", errors="replace")
        except OSError:
            problems.append("expected output content, but output file was missing")
        else:
            missing_output = [item for item in expected_output_contains if item not in output_text]
            if missing_output:
                problems.append("missing output text: " + ", ".join(missing_output))
            present_output = [item for item in expected_output_missing if item in output_text]
            if present_output:
                problems.append("unexpected output text present: " + ", ".join(present_output))

    if problems:
        return CaseResult(name, "FAIL", expected_exit, completed.returncode, case_out, "; ".join(problems))
    return CaseResult(name, "PASS", expected_exit, completed.returncode, case_out, str(case.get("lesson", "")))


def write_summary(out_dir: Path, results: list[CaseResult], cases: list[dict[str, Any]]) -> None:
    summary = out_dir / "summary.md"
    lesson_by_name = {str(case["name"]): str(case.get("lesson", "")) for case in cases}
    with summary.open("w", encoding="utf-8") as stream:
        stream.write("# p101-tool-playground corpus\n\n")
        stream.write("| Status | Case | Expected exit | Actual exit | Report | Lesson |\n")
        stream.write("| --- | --- | ---: | ---: | --- | --- |\n")
        for result in results:
            rel_report = result.report_dir.relative_to(out_dir)
            lesson = lesson_by_name.get(result.name, "")
            stream.write(f"| {result.status} | `{result.name}` | {result.expected_exit} | {result.actual_exit} | [{rel_report}/index.html](./{rel_report}/index.html) | {lesson} |\n")
        stream.write("\n## Logs\n\n")
        for result in results:
            stream.write(f"- `{result.name}`: [logs/{result.name}.log](./logs/{result.name}.log)\n")


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    root = Path(__file__).resolve().parents[1]
    cases_dir = root / "corpus" / "cases"
    out_dir = args.output or Path(f"/tmp/p101-tool-playground-corpus-{subprocess.os.getpid()}")
    out_dir = out_dir.resolve()
    if out_dir.exists():
        print(f"p101 corpus: output path already exists: {out_dir}", file=sys.stderr)
        return 2
    (out_dir / "logs").mkdir(parents=True)

    if args.quick:
        selected = {"clean", "fd-leak"}
    elif args.cases:
        selected = set(args.cases)
    else:
        selected = None

    cases = load_cases(cases_dir, selected, args.track)
    if not cases:
        print("p101 corpus: no cases selected", file=sys.stderr)
        return 2

    p101 = args.p101.resolve() if args.p101 else find_executable([root / "../scripts/p101", "p101"])
    playground = args.playground.resolve() if args.playground else find_executable(current_playground_candidates(root))

    results: list[CaseResult] = []
    for case in cases:
        result = run_case(root, p101, playground, out_dir, case, args.skip_html, args.skip_bundle)
        results.append(result)
        print(f"{result.status}: {result.name} ({result.message})")
        if result.status != "PASS" and not args.keep_going:
            break

    write_summary(out_dir, results, cases)
    print(f"p101 corpus output: {out_dir}")
    print(f"Summary: {out_dir / 'summary.md'}")
    return 0 if all(result.status == "PASS" for result in results) and len(results) == len(cases) else 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
