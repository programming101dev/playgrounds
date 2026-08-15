#!/usr/bin/env python3
"""Build a self-contained lab series from the playground corpus."""

from __future__ import annotations

import argparse
import html
import json
import os
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Any

@dataclass(frozen=True)
class LabCase:
    name: str
    order: int
    issue_id: str
    title: str
    category: str
    tracks: list[str]
    scenario: str
    scenario_behavior: str
    expected_exit: int
    expected_status: str
    lesson: str
    expected_findings: list[str]
    expects_error_path_findings: bool
    logic_issue_id: str
    expected_output_size: int | None
    fixed_output_size: int | None
    expected_output_contains: list[str]
    expected_output_missing: list[str]
    fixed_output_contains: list[str]
    fixed_output_not_contains: list[str]
    fix_goal: str
    fix_steps: list[str]
    case_dir: Path
    report_dir: Path


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Build the p101-tool-playground lab series.")
    parser.add_argument("-o", "--output", type=Path, help="Output directory. Default: /tmp/p101-tool-playground-lab-<pid>")
    parser.add_argument("--case", action="append", dest="cases", help="Include only this case name; may be repeated.")
    parser.add_argument("--track", choices=("c", "systems", "network"), help="Include only cases assigned to this playground track.")
    parser.add_argument("--quick", action="store_true", help="Use the short classroom set: clean and fd-leak.")
    parser.add_argument("--keep-going", action="store_true", help="Keep running corpus cases after a failed case.")
    parser.add_argument("--workflow", type=Path, help="Path to student-workflow.sh.")
    parser.add_argument("--playground", type=Path, help="Path to the p101-tool-playground executable.")
    parser.add_argument("--skip-html", action="store_true", help="Skip per-case p101 check HTML generation.")
    parser.add_argument("--skip-bundle", action="store_true", help="Skip per-case bug bundles.")
    parser.add_argument("--strict-corpus", action="store_true", help="Exit nonzero if the committed issue corpus no longer matches its oracle.")
    parser.add_argument("--require-all-fixed", action="store_true", help="Exit nonzero unless every selected issue lab is FIXED.")
    return parser.parse_args(argv)


def invocation_cwd() -> Path:
    return Path(os.environ.get("P101_INVOCATION_CWD", os.getcwd())).resolve()


def resolve_output(path: Path | None) -> Path:
    if path is None:
        return Path(f"/tmp/p101-tool-playground-lab-{os.getpid()}").resolve()
    if path.is_absolute():
        return path.resolve()
    return (invocation_cwd() / path).resolve()


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
    candidates.extend(
        [
            root / "build-clang/p101-tool-playground",
            root / "build-clang-22/p101-tool-playground",
            root / "build-gcc-16/p101-tool-playground",
            "p101-tool-playground",
        ]
    )
    return candidates


def read_text(path: Path, fallback: str = "") -> str:
    try:
        return path.read_text(encoding="utf-8")
    except OSError:
        return fallback


def read_json(path: Path) -> dict[str, Any]:
    data = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(data, dict):
        raise ValueError(f"{path} did not contain a JSON object")
    return data


def load_expected(path: Path) -> dict[str, Any]:
    data = read_json(path)
    data["case_dir"] = path.parent
    return data


def selected_case_names(args: argparse.Namespace) -> set[str] | None:
    if args.quick:
        return {"clean", "fd-leak"}
    if args.cases:
        return set(args.cases)
    return None


def expected_tracks(expected: dict[str, Any]) -> list[str]:
    tracks = expected.get("tracks", [])
    if isinstance(tracks, list):
        return [str(item) for item in tracks]
    if isinstance(tracks, str):
        return [tracks]
    return []


def load_scenario_manifest(playground: Path) -> dict[str, str]:
    completed = subprocess.run(
        [str(playground), "-S"],
        check=False,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True,
    )
    if completed.returncode != 0:
        raise ValueError(
            f"{playground} could not emit its scenario manifest: "
            f"{completed.stderr.strip()}"
        )
    lines = completed.stdout.splitlines()
    if not lines or lines[0] != "P101SCENARIOS\t1":
        raise ValueError(f"{playground} emitted an invalid scenario manifest")
    scenarios: dict[str, str] = {}
    for line_number, line in enumerate(lines[1:], 2):
        fields = line.split("\t")
        if len(fields) != 3 or not fields[0] or not fields[1]:
            raise ValueError(
                f"{playground}: scenario manifest line {line_number} is invalid"
            )
        if fields[0] in scenarios:
            raise ValueError(f"{playground}: duplicate scenario {fields[0]}")
        scenarios[fields[0]] = fields[1]
    if not scenarios:
        raise ValueError(f"{playground} emitted an empty scenario manifest")
    return scenarios


def load_cases(playground: Path, root: Path, runs_dir: Path, selected: set[str] | None, track: str | None) -> list[LabCase]:
    cases: list[LabCase] = []
    scenarios = load_scenario_manifest(playground)
    for expected_path in sorted((root / "corpus" / "cases").glob("*/expected.json")):
        expected = load_expected(expected_path)
        name = str(expected["name"])
        if selected is not None and name not in selected:
            continue
        tracks = expected_tracks(expected)
        if track is not None and track not in tracks:
            continue
        case_dir = Path(expected["case_dir"])
        scenario = str(expected["scenario"])
        if scenario not in scenarios:
            raise ValueError(f"{expected_path}: unknown scenario {scenario!r}")
        lesson = read_text(case_dir / "lesson.md", str(expected.get("lesson", ""))).strip()
        expected_findings = [str(item) for item in expected.get("expected_findings", [])]
        fix_steps = [str(item) for item in expected.get("fix_steps", [])]
        expected_output_size = expected.get("expected_output_size")
        fixed_output_size = expected.get("fixed_output_size")
        expected_output_contains = [str(item) for item in expected.get("expected_output_contains", [])]
        expected_output_missing = [str(item) for item in expected.get("expected_output_missing", [])]
        fixed_output_contains = [str(item) for item in expected.get("fixed_output_contains", [])]
        fixed_output_not_contains = [str(item) for item in expected.get("fixed_output_not_contains", [])]
        cases.append(
            LabCase(
                name=name,
                order=int(expected.get("lab_order", 1000)),
                issue_id=str(expected.get("issue_id", name)),
                title=str(expected.get("title", name)),
                category=str(expected.get("category", "")),
                tracks=tracks,
                scenario=scenario,
                scenario_behavior=scenarios[scenario],
                expected_exit=int(expected["expected_exit"]),
                expected_status=str(expected.get("expected_status", "")),
                lesson=lesson,
                expected_findings=expected_findings,
                expects_error_path_findings=bool(expected.get("expected_error_path_findings", False)),
                logic_issue_id=str(expected.get("logic_issue_id", "")),
                expected_output_size=int(expected_output_size) if expected_output_size is not None else None,
                fixed_output_size=int(fixed_output_size) if fixed_output_size is not None else None,
                expected_output_contains=expected_output_contains,
                expected_output_missing=expected_output_missing,
                fixed_output_contains=fixed_output_contains,
                fixed_output_not_contains=fixed_output_not_contains,
                fix_goal=str(expected.get("fix_goal", "")),
                fix_steps=fix_steps,
                case_dir=case_dir,
                report_dir=runs_dir / name,
            )
        )
    cases.sort(key=lambda item: (item.order, item.name))
    validate_case_identity(cases)
    return cases


def validate_case_identity(cases: list[LabCase]) -> None:
    orders: dict[int, str] = {}
    issue_ids: dict[str, str] = {}

    for case in cases:
        previous_order = orders.get(case.order)
        if previous_order is not None:
            raise ValueError(f"duplicate lab_order {case.order}: {previous_order} and {case.name}")
        orders[case.order] = case.name

        previous_issue = issue_ids.get(case.issue_id)
        if previous_issue is not None:
            raise ValueError(f"duplicate issue_id {case.issue_id}: {previous_issue} and {case.name}")
        issue_ids[case.issue_id] = case.name


def rel(from_dir: Path, path: Path) -> str:
    return path.relative_to(from_dir).as_posix()


def link_if_exists(out_dir: Path, path: Path, label: str) -> str:
    if not path.exists():
        return ""
    return f'<a href="{html.escape(rel(out_dir, path))}">{html.escape(label)}</a>'


def collect_correlated_findings(report_dir: Path) -> list[dict[str, Any]]:
    path = report_dir / "runtime" / "analysis" / "correlated-report.json"
    try:
        data = read_json(path)
    except (OSError, json.JSONDecodeError, ValueError):
        return []
    findings = data.get("findings")
    if not isinstance(findings, list):
        return []
    return [finding for finding in findings if isinstance(finding, dict)]


def collect_fault_findings(report_dir: Path) -> list[dict[str, Any]]:
    fault_dir = report_dir / "fault-walk"
    findings: list[dict[str, Any]] = []
    for path in sorted(fault_dir.glob("*.run/analysis/correlated-report.json")):
        try:
            data = read_json(path)
        except (OSError, json.JSONDecodeError, ValueError):
            continue
        items = data.get("findings")
        if isinstance(items, list):
            for item in items:
                if isinstance(item, dict):
                    item = dict(item)
                    item["source_report"] = path.parent.parent.name
                    findings.append(item)
    return findings


def issue_case(case: LabCase) -> bool:
    if case.expected_status == "PASS":
        return False
    return (
        bool(case.expected_findings)
        or case.expects_error_path_findings
        or case.expected_output_size is not None
        or bool(case.expected_output_contains)
        or bool(case.expected_output_missing)
    )


def parseable_json(path: Path) -> bool:
    try:
        read_json(path)
    except (OSError, json.JSONDecodeError, ValueError):
        return False
    return True


def missing_evidence(case: LabCase) -> list[str]:
    missing: list[str] = []
    if not case.report_dir.exists():
        return ["case report directory"]
    if case.expected_findings and not parseable_json(case.report_dir / "runtime" / "analysis" / "correlated-report.json"):
        missing.append("ordinary correlated-report.json")
    if case.expects_error_path_findings:
        fault_dir = case.report_dir / "fault-walk"
        if not fault_dir.exists() or not any(fault_dir.glob("*.run/analysis/correlated-report.json")):
            missing.append("fault-walk reports")
    if (case.expected_output_size is not None or case.expected_output_contains or case.expected_output_missing) and not (case.report_dir / "playground-output.txt").exists():
        missing.append("playground output")
    return missing


def shift_markdown_headings(text: str, minimum_level: int) -> str:
    lines: list[str] = []
    for line in text.splitlines():
        stripped = line.lstrip()
        indent = line[: len(line) - len(stripped)]
        if stripped.startswith("#"):
            hashes = len(stripped) - len(stripped.lstrip("#"))
            if hashes > 0 and len(stripped) > hashes and stripped[hashes] == " ":
                hashes = max(hashes, minimum_level)
                lines.append(f"{indent}{'#' * hashes}{stripped[stripped.find(' '):]}")
                continue
        lines.append(line)
    return "\n".join(lines)


def lesson_excerpt(text: str) -> str:
    lines: list[str] = []
    for line in text.splitlines():
        stripped = line.strip()
        if not stripped:
            continue
        if stripped.startswith("#"):
            continue
        lines.append(stripped)
    return " ".join(lines)


def format_finding(finding: dict[str, Any]) -> str:
    finding_id = str(finding.get("id", "P101-UNKNOWN"))
    message = str(finding.get("message", finding.get("kind", "")))
    location = ""
    lesson_link = ""
    if isinstance(finding.get("source_report"), str):
        location = f" ({finding['source_report']})"
    lesson = finding.get("lesson")
    if isinstance(lesson, dict) and isinstance(lesson.get("primary"), dict):
        primary = lesson["primary"]
        title = html.escape(str(primary.get("title", "lesson")))
        url = html.escape(str(primary.get("url", "")), quote=True)
        if url:
            lesson_link = f' — <a href="{url}">Learn how to fix this: {title}</a>'
    return f"<li><code>{html.escape(finding_id)}</code>{html.escape(location)} — {html.escape(message)}{lesson_link}</li>"


def case_status(case: LabCase) -> str:
    if not case.report_dir.exists():
        return "missing"
    if collect_correlated_findings(case.report_dir) or collect_fault_findings(case.report_dir):
        return "findings"
    return "clean"


def expected_issue_is_present(case: LabCase) -> bool:
    ordinary_ids = {str(finding.get("id", "")) for finding in collect_correlated_findings(case.report_dir)}
    fault_findings = collect_fault_findings(case.report_dir) if case.expects_error_path_findings else []
    if case.expected_findings and any(finding_id in ordinary_ids for finding_id in case.expected_findings):
        return True
    if case.expects_error_path_findings and fault_findings:
        return True
    if case.expected_output_size is not None:
        output_path = case.report_dir / "playground-output.txt"
        try:
            return output_path.stat().st_size == case.expected_output_size
        except OSError:
            return False
    if case.expected_output_contains or case.expected_output_missing:
        output_path = case.report_dir / "playground-output.txt"
        try:
            output_text = output_path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            return False
        contains_match = not case.expected_output_contains or all(
            item in output_text for item in case.expected_output_contains
        )
        missing_match = not case.expected_output_missing or all(
            item not in output_text for item in case.expected_output_missing
        )
        if contains_match and missing_match:
            return True
    return False


def has_fixed_oracle(case: LabCase) -> bool:
    return bool(case.fixed_output_contains or case.fixed_output_not_contains) or case.fixed_output_size is not None


def fixed_issue_is_demonstrated(case: LabCase) -> bool:
    """True when every fixed_* oracle the case declares is satisfied by the run output."""
    output_path = case.report_dir / "playground-output.txt"
    if case.fixed_output_size is not None:
        try:
            if output_path.stat().st_size != case.fixed_output_size:
                return False
        except OSError:
            return False
    if case.fixed_output_contains or case.fixed_output_not_contains:
        try:
            output_text = output_path.read_text(encoding="utf-8", errors="replace")
        except OSError:
            return False
        if any(item not in output_text for item in case.fixed_output_contains):
            return False
        if any(item in output_text for item in case.fixed_output_not_contains):
            return False
    return True


def lab_progress(case: LabCase) -> str:
    if not issue_case(case):
        return "REFERENCE"
    if missing_evidence(case):
        return "BLOCKED"
    if expected_issue_is_present(case):
        return "OPEN"
    if has_fixed_oracle(case):
        return "FIXED" if fixed_issue_is_demonstrated(case) else "INDETERMINATE"
    return "FIXED"


def progress_counts(cases: list[LabCase]) -> tuple[int, int, int, int, int]:
    issue_cases = [case for case in cases if issue_case(case)]
    fixed = sum(1 for case in issue_cases if lab_progress(case) == "FIXED")
    open_count = sum(1 for case in issue_cases if lab_progress(case) == "OPEN")
    blocked = sum(1 for case in issue_cases if lab_progress(case) == "BLOCKED")
    indeterminate = sum(1 for case in issue_cases if lab_progress(case) == "INDETERMINATE")
    return fixed, open_count, blocked, indeterminate, len(issue_cases)


def case_phase(case: LabCase) -> str:
    if case.tracks == ["systems"] or ("systems" in case.tracks and "c" not in case.tracks):
        if case.order <= 4:
            return "descriptor ownership"
        if case.order <= 15:
            return "process and error-path cleanup"
        if case.order <= 30:
            return "file I/O and syscall behavior"
        if case.order <= 42:
            return "file-system and OS security"
        if case.order <= 48:
            return "concurrency and multiprocessing"
        return "verification practice"
    if case.order == 0:
        return "reference"
    if case.order <= 4:
        return "resource ownership"
    if case.order <= 10:
        return "memory lifetime"
    if case.order <= 15:
        return "error-path cleanup"
    if case.order <= 30:
        return "checked state, bounds, and integers"
    if case.order <= 42:
        return "adversarial input and file security"
    if case.order <= 45:
        return "logging and observability"
    if case.order <= 48:
        return "concurrency and races"
    return "verification practice"


def case_difficulty(case: LabCase) -> str:
    if case.order == 0:
        return "reference"
    if case.order <= 12:
        return "intro"
    if case.order <= 34:
        return "medium"
    return "hard"


def case_minutes(case: LabCase) -> int:
    if case.order == 0:
        return 5
    if case_difficulty(case) == "intro":
        return 10
    if case_difficulty(case) == "medium":
        return 15
    return 25


def case_hints(case: LabCase) -> list[str]:
    hints = [
        f"Start in the `{case.scenario}` branch of `src/playground.c`.",
        f"The invariant is in the expected signal: `{case.logic_issue_id or ', '.join(case.expected_findings) or case.expected_status}`.",
    ]
    if case.fix_steps:
        hints.append(case.fix_steps[-1])
    else:
        hints.append("Use the clean baseline as the model for the repair.")
    return hints


def case_answer_key(case: LabCase) -> str:
    targets: list[str] = []
    if case.fixed_output_contains:
        targets.append("emit: " + ", ".join(f"`{item}`" for item in case.fixed_output_contains))
    if case.fixed_output_not_contains:
        targets.append("remove: " + ", ".join(f"`{item}`" for item in case.fixed_output_not_contains))
    if case.fixed_output_size is not None:
        targets.append(f"fixed output size: `{case.fixed_output_size}` bytes")
    if not targets and case.expected_findings:
        targets.append("remove finding IDs: " + ", ".join(f"`{item}`" for item in case.expected_findings))
    if case.expects_error_path_findings:
        targets.append("make the injected-failure walk clean")
    if not targets:
        targets.append("keep the reference run clean")
    return "; ".join(targets)


def phase_rows(cases: list[LabCase]) -> list[str]:
    rows: list[str] = []
    current_phase = ""
    start = 0
    end = 0
    for case in cases:
        phase = case_phase(case)
        if current_phase == "":
            current_phase = phase
            start = case.order
            end = case.order
            continue
        if phase == current_phase:
            end = case.order
            continue
        rows.append(f"| {start}–{end} | {current_phase} |")
        current_phase = phase
        start = case.order
        end = case.order
    if current_phase:
        rows.append(f"| {start}–{end} | {current_phase} |")
    return rows


def render_markdown(out_dir: Path, cases: list[LabCase], corpus_rc: int) -> str:
    fixed, open_count, blocked, indeterminate, total = progress_counts(cases)
    lines = [
        "# p101 tool playground lab series",
        "",
        "This is a series of small, intentionally broken p101 programs inside the playground. Fix one issue, re-run the lab, and watch that lab move from OPEN to FIXED.",
        "",
        f"- Progress: {fixed}/{total} issue labs fixed, {open_count} still open, {indeterminate} indeterminate, {blocked} blocked",
        f"- Instructor corpus oracle: {'PASS' if corpus_rc == 0 else 'CHANGED'}",
        f"- Corpus summary: [runs/summary.md](./runs/summary.md)",
        f"- HTML lab book: [index.html](./index.html)",
        "",
        "Use `./lab.sh --track c`, `./lab.sh --track systems`, or `./lab.sh --track network` for coarse lab slices. The full wrapper curriculum map lives in `tracks/README.md`.",
        "",
        "## Student workflow",
        "",
        "1. Run `./lab.sh` and open the first `OPEN` lab.",
        "2. Edit the matching scenario function in `src/playground.c`.",
        "3. Build with `cmake --build build` if you changed C code.",
        "4. Re-run `./lab.sh` and watch that lab move from `OPEN` to `FIXED`.",
        "5. Submit with `./submit-labs.sh` when your assigned labs are green.",
        "",
        "If you get lost, run `./reset-labs.sh --show` to preview the reset command, or `./reset-labs.sh --yes` to restore the committed lab fixtures.",
        "",
        "## Phase map",
        "",
        "| Labs | Phase |",
        "| ---: | --- |",
        *phase_rows(cases),
        "",
        "## Progress board",
        "",
        "| Lab | Status | Tracks | Phase | Difficulty | Time | Issue | Scenario | Expected signal |",
        "| ---: | --- | --- | --- | --- | ---: | --- | --- | --- |",
    ]
    for case in cases:
        signal = ", ".join(case.expected_findings)
        if case.expects_error_path_findings:
            signal = (signal + ", " if signal else "") + "error-path findings"
        if case.logic_issue_id:
            signal = (signal + ", " if signal else "") + case.logic_issue_id
        if case.expected_output_contains:
            signal = (signal + ", " if signal else "") + "output contains " + "; ".join(case.expected_output_contains)
        if case.expected_output_missing:
            signal = (signal + ", " if signal else "") + "output missing " + "; ".join(case.expected_output_missing)
        if not signal:
            signal = "clean reference"
        lines.append(f"| {case.order} | `{lab_progress(case)}` | {', '.join(case.tracks)} | {case_phase(case)} | {case_difficulty(case)} | {case_minutes(case)} min | {case.issue_id}: {case.title} | `{case.scenario}` | {signal} |")
    lines.extend(
        [
            "",
            "## Classroom arc",
            "",
            "1. Start with the clean tour and inspect what healthy ownership looks like.",
            "2. Introduce normal-path resource mistakes: leaked descriptors, leaked heap blocks, double close, and stray close.",
            "3. Move into memory lifetime: double free, stray free, use-after-free, and realloc ownership.",
            "4. Fix error-path cleanup: early returns, partial cleanup, and fault-injected failures.",
            "5. Check program state before use: NULL results and uninitialized values.",
            "6. Practice bounds, byte counts, strings, and integer-size traps.",
            "7. Treat input as adversarial: path traversal, command construction, format strings, temporary files, and resource limits.",
            "8. Make behavior observable without leaking secrets: structured logs, neutralized fields, and safe logging.",
            "9. Finish with concurrency hazards and parser fuzzing.",
            "",
            "## Lessons",
            "",
        ]
    )
    for case in cases:
        lines.extend(
            [
                f"### Lab {case.order}: {case.title}",
                "",
                f"- Issue ID: `{case.issue_id}`",
                f"- Category: `{case.category}`",
                f"- Tracks: `{', '.join(case.tracks)}`",
                f"- Phase: `{case_phase(case)}`",
                f"- Difficulty: `{case_difficulty(case)}`",
                f"- Estimated time: `{case_minutes(case)} minutes`",
                f"- Scenario: `{case.scenario}`",
                f"- Evidence kind: `{case.scenario_behavior}`",
                f"- Expected status: `{case.expected_status}`",
                f"- Expected exit: `{case.expected_exit}`",
                f"- Progress: `{lab_progress(case)}`",
                f"- Report: [runs/{case.name}/index.html](./runs/{case.name}/index.html)",
                "",
                f"Goal: {case.fix_goal}",
                "",
                "Fix checklist:",
                "",
                *[f"- {step}" for step in case.fix_steps],
                "",
                "Hints:",
                "",
                *[f"- {hint}" for hint in case_hints(case)],
                "",
                "Instructor answer key signal:",
                "",
                f"- {case_answer_key(case)}",
                "",
                shift_markdown_headings(case.lesson, 4),
                "",
            ]
        )
    lines.append("## Generated files")
    lines.append("")
    lines.append("- `runs/`: checked corpus output")
    lines.append("- `logs/corpus.log`: command transcript for the corpus run")
    lines.append("- `index.html`: self-contained instructor/student lab view")
    lines.append("- `./reset-labs.sh`: restore committed fixtures after an experiment goes sideways")
    lines.append("- `./submit-labs.sh`: run the student-facing build/test/lab checks")
    lines.append("")
    return "\n".join(lines)


def render_html(out_dir: Path, cases: list[LabCase], corpus_rc: int) -> str:
    status = "PASS" if corpus_rc == 0 else "FAIL"
    fixed, open_count, blocked, indeterminate, total = progress_counts(cases)
    cards: list[str] = []
    for case in cases:
        findings = collect_correlated_findings(case.report_dir)
        fault_findings = collect_fault_findings(case.report_dir) if case.expects_error_path_findings else []
        progress = lab_progress(case)
        finding_items = "".join(format_finding(finding) for finding in findings[:8])
        fault_items = "".join(format_finding(finding) for finding in fault_findings[:8])
        if not finding_items:
            finding_items = "<li>No ordinary-run findings.</li>"
        if case.expects_error_path_findings and not fault_items:
            fault_items = "<li>Error-path findings were expected, but none were summarized here. Check the linked fault-walk output.</li>"
        elif not case.expects_error_path_findings:
            fault_items = "<li>This lesson is about the ordinary run; use the error-path lesson for injected-failure behavior.</li>"
        elif not fault_items:
            fault_items = "<li>No summarized fault-walk findings.</li>"
        if progress == "BLOCKED":
            missing = ", ".join(missing_evidence(case))
            fault_items = f"<li>This lab could not be judged because evidence is missing: {html.escape(missing)}.</li>"
        progress_note = ""
        if progress == "INDETERMINATE":
            progress_note = '<p class="muted">The broken signal is gone, but the fixed-output oracle is not satisfied yet, so this lab does not count as fixed.</p>'

        expected = " ".join(f"<code>{html.escape(item)}</code>" for item in case.expected_findings)
        if case.expects_error_path_findings:
            expected = (expected + " " if expected else "") + "<code>error-path findings</code>"
        if case.logic_issue_id:
            expected = (expected + " " if expected else "") + f"<code>{html.escape(case.logic_issue_id)}</code>"
        if case.expected_output_size is not None:
            expected = (expected + " " if expected else "") + f"<code>{case.expected_output_size} bytes</code>"
        if case.fixed_output_size is not None:
            expected = expected + f" <span class=\"muted\">fixed target: {case.fixed_output_size} bytes</span>"
        if case.expected_output_contains:
            expected = (expected + " " if expected else "") + " ".join(f"<code>contains {html.escape(item)}</code>" for item in case.expected_output_contains)
        if case.expected_output_missing:
            expected = (expected + " " if expected else "") + " ".join(f"<code>missing {html.escape(item)}</code>" for item in case.expected_output_missing)
        if case.fixed_output_contains:
            expected = expected + " <span class=\"muted\">fixed contains: " + ", ".join(html.escape(item) for item in case.fixed_output_contains) + "</span>"
        if case.fixed_output_not_contains:
            expected = expected + " <span class=\"muted\">fixed excludes: " + ", ".join(html.escape(item) for item in case.fixed_output_not_contains) + "</span>"
        if not expected:
            expected = "<code>clean</code>"

        steps = "".join(f"<li>{html.escape(step)}</li>" for step in case.fix_steps)
        if not steps:
            steps = "<li>Use the linked report to identify the missing ownership step.</li>"

        links = [
            link_if_exists(out_dir, case.report_dir / "index.html", "case HTML report"),
            link_if_exists(out_dir, case.report_dir / "summary.md", "case summary"),
            link_if_exists(out_dir, case.report_dir / "runtime" / "analysis" / "correlated-report.txt", "correlated text"),
            link_if_exists(out_dir, case.report_dir / "runtime" / "analysis" / "resource-lifetimes.md", "resource lifetimes"),
            link_if_exists(out_dir, case.report_dir / "fault-walk", "fault-walk directory"),
            link_if_exists(out_dir, case.report_dir / "lesson-guide.md", "lesson guide"),
            link_if_exists(out_dir, out_dir / "runs" / "logs" / f"{case.name}.log", "command log"),
        ]
        link_text = " · ".join(link for link in links if link)

        cards.append(
            f"""
            <article class="card {html.escape(progress.lower())}">
              <div class="card-title">
                <h3>Lab {case.order}: {html.escape(case.title)}</h3>
                <span>{html.escape(progress)}</span>
              </div>
              <p><code>{html.escape(case.issue_id)}</code> · tracks {html.escape(', '.join(case.tracks))} · {html.escape(case_phase(case))} · {html.escape(case_difficulty(case))} · about {case_minutes(case)} min · scenario <code>{html.escape(case.scenario)}</code> · evidence <code>{html.escape(case.scenario_behavior)}</code></p>
              <p class="lesson">{html.escape(lesson_excerpt(case.lesson))}</p>
              {progress_note}
              <dl>
                <dt>Goal</dt><dd>{html.escape(case.fix_goal)}</dd>
                <dt>Expected</dt><dd>{expected}</dd>
                <dt>Answer signal</dt><dd>{html.escape(case_answer_key(case))}</dd>
                <dt>Exit oracle</dt><dd><code>{case.expected_exit}</code> / <code>{html.escape(case.expected_status)}</code></dd>
              </dl>
              <h4>Fix checklist</h4>
              <ol>{steps}</ol>
              <h4>Hints</h4>
              <ul>{"".join(f"<li>{html.escape(hint)}</li>" for hint in case_hints(case))}</ul>
              <h4>Ordinary run</h4>
              <ul>{finding_items}</ul>
              <h4>Injected error paths</h4>
              <ul>{fault_items}</ul>
              <p class="links">{link_text}</p>
            </article>
            """
        )

    css = """
    :root { color-scheme: light dark; --ok: #207044; --bad: #a73535; --ref: #2868c7; --wip: #7a4fbf; --ink: #1f2933; --muted: #65717f; --card: #ffffff; --line: #d9e2ec; --bg: #f5f7fa; }
    @media (prefers-color-scheme: dark) { :root { --ink: #ecf2f8; --muted: #aab7c4; --card: #121820; --line: #27313d; --bg: #0b1016; } }
    body { background: var(--bg); color: var(--ink); font: 16px/1.55 -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif; margin: 0; }
    main { max-width: 1120px; margin: 0 auto; padding: 2.5rem 1.25rem 4rem; }
    h1 { font-size: clamp(2rem, 5vw, 4rem); letter-spacing: -0.04em; margin: 0 0 .5rem; }
    h2 { margin-top: 2.5rem; }
    a { color: #2868c7; }
    code { background: rgba(127, 127, 127, .14); border-radius: .35rem; padding: .08rem .28rem; }
    .hero { background: linear-gradient(135deg, rgba(57, 123, 222, .18), rgba(32, 112, 68, .14)); border: 1px solid var(--line); border-radius: 1.25rem; padding: 1.5rem; }
    .pill { display: inline-block; border-radius: 999px; padding: .3rem .7rem; background: var(--card); border: 1px solid var(--line); color: var(--muted); font-size: .9rem; }
    .grid { display: grid; gap: 1rem; grid-template-columns: repeat(auto-fit, minmax(310px, 1fr)); }
    .card { background: var(--card); border: 1px solid var(--line); border-radius: 1rem; padding: 1rem; box-shadow: 0 12px 30px rgba(15, 23, 42, .06); }
    .card.clean { border-top: .35rem solid var(--ok); }
    .card.reference { border-top: .35rem solid var(--ref); }
    .card.fixed { border-top: .35rem solid var(--ok); }
    .card.open { border-top: .35rem solid var(--bad); }
    .card.indeterminate { border-top: .35rem solid var(--wip); }
    .card.blocked { border-top: .35rem solid #a15c00; }
    .card.findings { border-top: .35rem solid var(--bad); }
    .card.missing { border-top: .35rem solid #a15c00; }
    .card-title { display: flex; align-items: baseline; justify-content: space-between; gap: 1rem; }
    .card-title h3 { margin: 0; }
    .card-title span { color: var(--muted); text-transform: uppercase; font-size: .8rem; letter-spacing: .08em; }
    .lesson { min-height: 3em; }
    dl { display: grid; grid-template-columns: 7rem 1fr; gap: .25rem .75rem; }
    dt { color: var(--muted); }
    dd { margin: 0; }
    .links, .muted { color: var(--muted); }
    .flow { display: grid; gap: .8rem; grid-template-columns: repeat(auto-fit, minmax(210px, 1fr)); margin-top: 1rem; }
    .step { background: var(--card); border: 1px solid var(--line); border-radius: .85rem; padding: .9rem; }
    """

    return f"""<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>p101 tool playground lab series</title>
  <style>{css}</style>
</head>
<body>
<main>
  <section class="hero">
    <p class="pill">Generated lab series · {fixed}/{total} fixed · corpus {html.escape(status)}</p>
    <h1>p101 tool playground lab series</h1>
    <p>This is the 10x playground: a sequence of intentionally broken, fixable labs. Students fix one issue at a time, re-run the lab, and see the progress board move from OPEN to FIXED.</p>
    <p>Use <code>./lab.sh --track c</code>, <code>./lab.sh --track systems</code>, or <code>./lab.sh --track network</code> for coarse lab slices. The full wrapper curriculum map lives in <code>tracks/README.md</code>.</p>
    <p><a href="runs/summary.md">Corpus summary</a> · <a href="lab.md">Markdown lab</a> · <a href="logs/corpus.log">Corpus command log</a></p>
  </section>

  <h2>Progress board</h2>
  <section class="flow">
    <div class="step"><strong>{fixed}/{total} fixed</strong><br>{open_count} issue labs still open; {indeterminate} indeterminate (broken signal gone, fix not demonstrated); {blocked} blocked by missing evidence.</div>
    <div class="step"><strong>How to use it</strong><br>Fix one lab, rebuild if needed, run <code>./lab.sh</code>, and refresh this page.</div>
    <div class="step"><strong>Instructor oracle</strong><br>{html.escape(status)} means the committed broken fixtures still demonstrate the expected issues.</div>
    <div class="step"><strong>Recovery</strong><br>Run <code>./reset-labs.sh --show</code> to preview reset, or <code>./reset-labs.sh --yes</code> to restore fixtures.</div>
    <div class="step"><strong>Submission</strong><br>Run <code>./submit-labs.sh</code> before handing in the lab.</div>
  </section>

  <h2>Classroom arc</h2>
  <section class="flow">
    <div class="step"><strong>1. Observe clean code.</strong><br>Start with the clean tour and inspect the trace/resource model.</div>
    <div class="step"><strong>2. Own resources deliberately.</strong><br>Compare descriptor leaks, heap leaks, double release, stale ownership, and realloc handling.</div>
    <div class="step"><strong>3. Make error paths honest.</strong><br>Early returns and injected failures must still release every owned resource.</div>
    <div class="step"><strong>4. Check before use.</strong><br>Handle NULL, initialization, bounds, byte counts, strings, and integer sizes before touching memory.</div>
    <div class="step"><strong>5. Assume adversarial input.</strong><br>Reject traversal, command construction, format-string misuse, predictable temp files, and unbounded requests.</div>
    <div class="step"><strong>6. Log safely.</strong><br>Use structured logs, neutralize untrusted text, and never emit secrets.</div>
    <div class="step"><strong>7. Finish with concurrency and verification.</strong><br>Study TOCTOU/data races, then add fuzzing around parser boundaries.</div>
  </section>

  <h2>Lessons</h2>
  <section class="grid">
    {''.join(cards)}
  </section>
</main>
</body>
</html>
"""


def run_corpus(root: Path, out_dir: Path, args: argparse.Namespace) -> int:
    runs_dir = out_dir / "runs"
    command = [sys.executable, str(root / "corpus" / "run-corpus.py"), "-o", str(runs_dir)]
    command.append("--keep-going")
    if args.quick:
        command.append("--quick")
    if args.track is not None:
        command.extend(["--track", args.track])
    for case_name in args.cases or []:
        command.extend(["--case", case_name])
    if args.workflow is not None:
        command.extend(["--workflow", str(args.workflow)])
    if args.playground is not None:
        command.extend(["--playground", str(args.playground)])
    if args.skip_html:
        command.append("--skip-html")
    if args.skip_bundle:
        command.append("--skip-bundle")
    if args.strict_corpus:
        command.append("--strict")

    logs_dir = out_dir / "logs"
    logs_dir.mkdir()
    log_path = logs_dir / "corpus.log"
    with log_path.open("w", encoding="utf-8") as log:
        log.write("$ " + " ".join(command) + "\n\n")
        log.flush()
        completed = subprocess.run(command, cwd=root, stdout=log, stderr=subprocess.STDOUT, check=False)
    return completed.returncode


def find_audit_doctor(root: Path) -> Path | None:
    candidates: list[Path | str] = []
    configured = os.environ.get("P101_AUDIT_DOCTOR", "").strip()
    if configured:
        candidates.append(configured)
    candidates.extend([
        root / "../programs/p101-audit/build-clang-22/audit-doctor",
        root / "../programs/p101-audit/build-clang/audit-doctor",
        root / "../programs/p101-audit/build-gcc-16/audit-doctor",
    ])
    for candidate in candidates:
        path = Path(candidate).expanduser().resolve()
        if path.is_file() and os.access(path, os.X_OK):
            return path
    resolved = shutil.which("audit-doctor")
    return Path(resolved).resolve() if resolved is not None else None


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    root = Path(__file__).resolve().parents[1]
    out_dir = resolve_output(args.output)
    if out_dir.exists():
        print(f"p101 playground lab: output path already exists: {out_dir}", file=sys.stderr)
        return 2
    out_dir.mkdir(parents=True)

    try:
        playground = args.playground.resolve() if args.playground else find_executable(current_playground_candidates(root))
        cases = load_cases(playground, root, out_dir / "runs", selected_case_names(args), args.track)
    except ValueError as exc:
        print(f"p101 playground lab: invalid corpus metadata: {exc}", file=sys.stderr)
        return 2
    if not cases:
        print("p101 playground lab: no cases selected", file=sys.stderr)
        return 2

    if find_audit_doctor(root) is None:
        print("p101 playground lab: audit-doctor is required to judge lab progress; build/install p101-audit first", file=sys.stderr)
        return 2

    corpus_rc = run_corpus(root, out_dir, args)

    (out_dir / "lab.md").write_text(render_markdown(out_dir, cases, corpus_rc), encoding="utf-8")
    (out_dir / "index.html").write_text(render_html(out_dir, cases, corpus_rc), encoding="utf-8")

    print(f"p101 playground lab output: {out_dir}")
    print(f"HTML: {out_dir / 'index.html'}")
    print(f"Markdown: {out_dir / 'lab.md'}")
    if corpus_rc != 0:
        print(f"Corpus oracle changed; see {out_dir / 'logs' / 'corpus.log'}", file=sys.stderr)
    fixed, open_count, blocked, indeterminate, total = progress_counts(cases)
    if blocked > 0:
        print(f"Lab progress is blocked: {blocked} issue labs are missing evidence", file=sys.stderr)
        return 2
    if args.require_all_fixed and (open_count > 0 or indeterminate > 0):
        print(
            f"Lab progress is incomplete: {open_count} of {total} issue labs are still OPEN "
            f"and {indeterminate} are INDETERMINATE (broken signal gone, fix not demonstrated)",
            file=sys.stderr,
        )
        return 1
    return corpus_rc if args.strict_corpus else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
