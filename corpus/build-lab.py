#!/usr/bin/env python3
"""Build a self-contained lab series from the playground corpus."""

from __future__ import annotations

import argparse
import html
import json
import os
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
    scenario: str
    expected_exit: int
    expected_status: str
    lesson: str
    expected_findings: list[str]
    expects_error_path_findings: bool
    logic_issue_id: str
    expected_output_size: int | None
    fixed_output_size: int | None
    fix_goal: str
    fix_steps: list[str]
    case_dir: Path
    report_dir: Path


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Build the p101-tool-playground lab series.")
    parser.add_argument("-o", "--output", type=Path, help="Output directory. Default: /tmp/p101-tool-playground-lab-<pid>")
    parser.add_argument("--case", action="append", dest="cases", help="Include only this case name; may be repeated.")
    parser.add_argument("--quick", action="store_true", help="Use the short classroom set: clean and fd-leak.")
    parser.add_argument("--keep-going", action="store_true", help="Keep running corpus cases after a failed case.")
    parser.add_argument("--p101", type=Path, help="Path to the p101 dispatcher.")
    parser.add_argument("--playground", type=Path, help="Path to the p101-tool-playground executable.")
    parser.add_argument("--skip-html", action="store_true", help="Skip per-case p101 check HTML generation.")
    parser.add_argument("--skip-bundle", action="store_true", help="Skip per-case bug bundles.")
    parser.add_argument("--strict-corpus", action="store_true", help="Exit nonzero if the committed issue corpus no longer matches its oracle.")
    return parser.parse_args(argv)


def invocation_cwd() -> Path:
    return Path(os.environ.get("P101_DISPATCH_CWD", os.getcwd())).resolve()


def resolve_output(path: Path | None) -> Path:
    if path is None:
        return Path(f"/tmp/p101-tool-playground-lab-{os.getpid()}").resolve()
    if path.is_absolute():
        return path.resolve()
    return (invocation_cwd() / path).resolve()


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


def load_cases(root: Path, runs_dir: Path, selected: set[str] | None) -> list[LabCase]:
    cases: list[LabCase] = []
    for expected_path in sorted((root / "corpus" / "cases").glob("*/expected.json")):
        expected = load_expected(expected_path)
        name = str(expected["name"])
        if selected is not None and name not in selected:
            continue
        case_dir = Path(expected["case_dir"])
        lesson = read_text(case_dir / "lesson.md", str(expected.get("lesson", ""))).strip()
        expected_findings = [str(item) for item in expected.get("expected_findings", [])]
        fix_steps = [str(item) for item in expected.get("fix_steps", [])]
        expected_output_size = expected.get("expected_output_size")
        fixed_output_size = expected.get("fixed_output_size")
        cases.append(
            LabCase(
                name=name,
                order=int(expected.get("lab_order", 1000)),
                issue_id=str(expected.get("issue_id", name)),
                title=str(expected.get("title", name)),
                category=str(expected.get("category", "")),
                scenario=str(expected["scenario"]),
                expected_exit=int(expected["expected_exit"]),
                expected_status=str(expected.get("expected_status", "")),
                lesson=lesson,
                expected_findings=expected_findings,
                expects_error_path_findings=bool(expected.get("expected_error_path_findings", False)),
                logic_issue_id=str(expected.get("logic_issue_id", "")),
                expected_output_size=int(expected_output_size) if expected_output_size is not None else None,
                fixed_output_size=int(fixed_output_size) if fixed_output_size is not None else None,
                fix_goal=str(expected.get("fix_goal", "")),
                fix_steps=fix_steps,
                case_dir=case_dir,
                report_dir=runs_dir / name,
            )
        )
    cases.sort(key=lambda item: (item.order, item.name))
    return cases


def rel(from_dir: Path, path: Path) -> str:
    return path.relative_to(from_dir).as_posix()


def link_if_exists(out_dir: Path, path: Path, label: str) -> str:
    if not path.exists():
        return ""
    return f'<a href="{html.escape(rel(out_dir, path))}">{html.escape(label)}</a>'


def collect_correlated_findings(report_dir: Path) -> list[dict[str, Any]]:
    path = report_dir / "doctor" / "observe" / "correlated-report.json"
    try:
        data = read_json(path)
    except (OSError, json.JSONDecodeError, ValueError):
        return []
    findings = data.get("findings")
    if not isinstance(findings, list):
        return []
    return [finding for finding in findings if isinstance(finding, dict)]


def collect_fault_findings(report_dir: Path) -> list[dict[str, Any]]:
    fault_dir = report_dir / "doctor" / "fault-walk"
    findings: list[dict[str, Any]] = []
    for path in sorted(fault_dir.glob("*.observe/correlated-report.json")):
        try:
            data = read_json(path)
        except (OSError, json.JSONDecodeError, ValueError):
            continue
        items = data.get("findings")
        if isinstance(items, list):
            for item in items:
                if isinstance(item, dict):
                    item = dict(item)
                    item["source_report"] = path.parent.name
                    findings.append(item)
    return findings


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
    if isinstance(finding.get("source_report"), str):
        location = f" ({finding['source_report']})"
    return f"<li><code>{html.escape(finding_id)}</code>{html.escape(location)} — {html.escape(message)}</li>"


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
    return False


def lab_progress(case: LabCase) -> str:
    if not case.report_dir.exists():
        return "MISSING"
    if not case.expected_findings and not case.expects_error_path_findings and case.expected_output_size is None:
        return "REFERENCE"
    return "OPEN" if expected_issue_is_present(case) else "FIXED"


def progress_counts(cases: list[LabCase]) -> tuple[int, int, int]:
    issue_cases = [case for case in cases if case.expected_findings or case.expects_error_path_findings or case.expected_output_size is not None]
    fixed = sum(1 for case in issue_cases if lab_progress(case) == "FIXED")
    open_count = sum(1 for case in issue_cases if lab_progress(case) == "OPEN")
    return fixed, open_count, len(issue_cases)


def render_markdown(out_dir: Path, cases: list[LabCase], corpus_rc: int) -> str:
    fixed, open_count, total = progress_counts(cases)
    lines = [
        "# p101 tool playground lab series",
        "",
        "This is a series of small, intentionally broken p101 programs inside the playground. Fix one issue, re-run the lab, and watch that lab move from OPEN to FIXED.",
        "",
        f"- Progress: {fixed}/{total} issue labs fixed, {open_count} still open",
        f"- Instructor corpus oracle: {'PASS' if corpus_rc == 0 else 'CHANGED'}",
        f"- Corpus summary: [runs/summary.md](./runs/summary.md)",
        f"- HTML lab book: [index.html](./index.html)",
        "",
        "## Progress board",
        "",
        "| Lab | Status | Issue | Scenario | Expected signal |",
        "| ---: | --- | --- | --- | --- |",
    ]
    for case in cases:
        signal = ", ".join(case.expected_findings)
        if case.expects_error_path_findings:
            signal = (signal + ", " if signal else "") + "error-path findings"
        if case.logic_issue_id:
            signal = (signal + ", " if signal else "") + case.logic_issue_id
        if not signal:
            signal = "clean reference"
        lines.append(f"| {case.order} | `{lab_progress(case)}` | {case.issue_id}: {case.title} | `{case.scenario}` | {signal} |")
    lines.extend(
        [
            "",
        "## Classroom arc",
        "",
        "1. Start with the clean tour and inspect what a healthy resource lifetime looks like.",
        "2. Introduce one broken ownership rule at a time: leaked descriptors, leaked heap blocks, double close, and stray close.",
            "3. Fix early-return cleanup so control-flow convenience does not skip ownership release.",
            "4. Fix multi-resource cleanup by treating cleanup as a ledger.",
            "5. Run the fault lab to show the uncomfortable truth: the happy path can be clean while the error path is vulnerable.",
            "6. Use the linked reports to connect each finding back to source and call history.",
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
                f"- Scenario: `{case.scenario}`",
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
                shift_markdown_headings(case.lesson, 4),
                "",
            ]
        )
    lines.append("## Generated files")
    lines.append("")
    lines.append("- `runs/`: checked corpus output")
    lines.append("- `logs/corpus.log`: command transcript for the corpus run")
    lines.append("- `index.html`: self-contained instructor/student lab view")
    lines.append("")
    return "\n".join(lines)


def render_html(out_dir: Path, cases: list[LabCase], corpus_rc: int) -> str:
    status = "PASS" if corpus_rc == 0 else "FAIL"
    fixed, open_count, total = progress_counts(cases)
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

        expected = " ".join(f"<code>{html.escape(item)}</code>" for item in case.expected_findings)
        if case.expects_error_path_findings:
            expected = (expected + " " if expected else "") + "<code>error-path findings</code>"
        if case.logic_issue_id:
            expected = (expected + " " if expected else "") + f"<code>{html.escape(case.logic_issue_id)}</code>"
        if case.expected_output_size is not None:
            expected = (expected + " " if expected else "") + f"<code>{case.expected_output_size} bytes</code>"
        if case.fixed_output_size is not None:
            expected = expected + f" <span class=\"muted\">fixed target: {case.fixed_output_size} bytes</span>"
        if not expected:
            expected = "<code>clean</code>"

        steps = "".join(f"<li>{html.escape(step)}</li>" for step in case.fix_steps)
        if not steps:
            steps = "<li>Use the linked report to identify the missing ownership step.</li>"

        links = [
            link_if_exists(out_dir, case.report_dir / "index.html", "case HTML report"),
            link_if_exists(out_dir, case.report_dir / "summary.md", "case summary"),
            link_if_exists(out_dir, case.report_dir / "doctor" / "observe" / "correlated-report.txt", "correlated text"),
            link_if_exists(out_dir, case.report_dir / "doctor" / "observe" / "resource-lifetimes.md", "resource lifetimes"),
            link_if_exists(out_dir, case.report_dir / "doctor" / "fault-walk", "fault-walk directory"),
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
              <p><code>{html.escape(case.issue_id)}</code> · {html.escape(case.category)} · scenario <code>{html.escape(case.scenario)}</code></p>
              <p class="lesson">{html.escape(lesson_excerpt(case.lesson))}</p>
              <dl>
                <dt>Goal</dt><dd>{html.escape(case.fix_goal)}</dd>
                <dt>Expected</dt><dd>{expected}</dd>
                <dt>Exit oracle</dt><dd><code>{case.expected_exit}</code> / <code>{html.escape(case.expected_status)}</code></dd>
              </dl>
              <h4>Fix checklist</h4>
              <ol>{steps}</ol>
              <h4>Ordinary run</h4>
              <ul>{finding_items}</ul>
              <h4>Injected error paths</h4>
              <ul>{fault_items}</ul>
              <p class="links">{link_text}</p>
            </article>
            """
        )

    css = """
    :root { color-scheme: light dark; --ok: #207044; --bad: #a73535; --ref: #2868c7; --ink: #1f2933; --muted: #65717f; --card: #ffffff; --line: #d9e2ec; --bg: #f5f7fa; }
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
    <p><a href="runs/summary.md">Corpus summary</a> · <a href="lab.md">Markdown lab</a> · <a href="logs/corpus.log">Corpus command log</a></p>
  </section>

  <h2>Progress board</h2>
  <section class="flow">
    <div class="step"><strong>{fixed}/{total} fixed</strong><br>{open_count} issue labs still open.</div>
    <div class="step"><strong>How to use it</strong><br>Fix one lab, rebuild if needed, run <code>./lab.sh</code>, and refresh this page.</div>
    <div class="step"><strong>Instructor oracle</strong><br>{html.escape(status)} means the committed broken fixtures still demonstrate the expected issues.</div>
  </section>

  <h2>Classroom arc</h2>
  <section class="flow">
    <div class="step"><strong>1. Observe clean code.</strong><br>Start with the clean tour and inspect the trace/resource model.</div>
    <div class="step"><strong>2. Break one ownership rule.</strong><br>Compare fd leaks, heap leaks, double close, and stray close.</div>
    <div class="step"><strong>3. Fix control flow.</strong><br>Early returns are fine only if cleanup still runs.</div>
    <div class="step"><strong>4. Inject failure.</strong><br>Use the fault lab to make cold error paths execute.</div>
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
    for case_name in args.cases or []:
        command.extend(["--case", case_name])
    if args.p101 is not None:
        command.extend(["--p101", str(args.p101)])
    if args.playground is not None:
        command.extend(["--playground", str(args.playground)])
    if args.skip_html:
        command.append("--skip-html")
    if args.skip_bundle:
        command.append("--skip-bundle")

    logs_dir = out_dir / "logs"
    logs_dir.mkdir()
    log_path = logs_dir / "corpus.log"
    with log_path.open("w", encoding="utf-8") as log:
        log.write("$ " + " ".join(command) + "\n\n")
        log.flush()
        completed = subprocess.run(command, cwd=root, stdout=log, stderr=subprocess.STDOUT, check=False)
    return completed.returncode


def main(argv: list[str]) -> int:
    args = parse_args(argv)
    root = Path(__file__).resolve().parents[1]
    out_dir = resolve_output(args.output)
    if out_dir.exists():
        print(f"p101 playground lab: output path already exists: {out_dir}", file=sys.stderr)
        return 2
    out_dir.mkdir(parents=True)

    corpus_rc = run_corpus(root, out_dir, args)
    cases = load_cases(root, out_dir / "runs", selected_case_names(args))
    if not cases:
        print("p101 playground lab: no cases selected", file=sys.stderr)
        return 2

    (out_dir / "lab.md").write_text(render_markdown(out_dir, cases, corpus_rc), encoding="utf-8")
    (out_dir / "index.html").write_text(render_html(out_dir, cases, corpus_rc), encoding="utf-8")

    print(f"p101 playground lab output: {out_dir}")
    print(f"HTML: {out_dir / 'index.html'}")
    print(f"Markdown: {out_dir / 'lab.md'}")
    if corpus_rc != 0:
        print(f"Corpus oracle changed; see {out_dir / 'logs' / 'corpus.log'}", file=sys.stderr)
    return corpus_rc if args.strict_corpus else 0


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
