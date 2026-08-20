#!/usr/bin/env python3
"""Regression tests for the playground lab receipt boundary."""

from __future__ import annotations

import importlib.util
import json
import sys
import tempfile
import unittest
from pathlib import Path


MODULE_PATH = Path(__file__).resolve().parents[1] / "corpus" / "build-lab.py"
sys.path.insert(0, str(MODULE_PATH.parent))
SPEC = importlib.util.spec_from_file_location("p101_build_lab", MODULE_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"cannot load {MODULE_PATH}")
BUILD_LAB = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = BUILD_LAB
SPEC.loader.exec_module(BUILD_LAB)


class CorpusReceiptTests(unittest.TestCase):
    def write_receipt(self, directory: Path, *, passed: bool = True) -> Path:
        receipt = directory / "receipt.json"
        receipt.write_text(
            "{\n"
            '  "schema": "p101-corpus-receipt-v1",\n'
            f'  "passed": {str(passed).lower()},\n'
            '  "strict": true,\n'
            f'  "fixtures": "{directory}",\n'
            '  "selected_cases": 1,\n'
            '  "completed_cases": 1,\n'
            '  "cases": [{"name": "clean", "status": "PASS", "report": "clean"}]\n'
            "}\n",
            encoding="utf-8",
        )
        return receipt

    def test_passed_receipt_is_admitted(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            receipt = self.write_receipt(root)
            document, cases = BUILD_LAB.load_receipt(receipt, root)
            self.assertTrue(document["passed"])
            self.assertEqual(set(cases), {"clean"})

    def test_failed_but_complete_receipt_is_admitted_for_student_progress(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            receipt = self.write_receipt(root, passed=False)
            document, cases = BUILD_LAB.load_receipt(receipt, root)
            self.assertFalse(document["passed"])
            self.assertEqual(set(cases), {"clean"})

    def test_report_path_cannot_escape_receipt_directory(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            receipt = root / "receipt.json"
            with self.assertRaisesRegex(ValueError, "escapes"):
                BUILD_LAB.receipt_report_dir(receipt, {"report": "../outside"})

    def test_complete_receipt_renders_fixture_without_executing_tools(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory) / "playgrounds"
            case_dir = root / "corpus" / "cases" / "clean"
            report_dir = root / "evidence" / "clean"
            analysis_dir = report_dir / "runtime" / "analysis"
            case_dir.mkdir(parents=True)
            analysis_dir.mkdir(parents=True)
            (case_dir / "lesson.md").write_text("# Clean lesson\n", encoding="utf-8")
            (case_dir / "expected.json").write_text(
                json.dumps(
                    {
                        "name": "clean",
                        "lab_order": 0,
                        "issue_id": "P101-LAB-000",
                        "title": "Clean",
                        "category": "baseline",
                        "tracks": ["c"],
                        "scenario": "tour",
                        "expected_exit": 0,
                        "expected_status": "PASS",
                        "expected_findings": [],
                        "fix_goal": "Stay clean.",
                        "fix_steps": [],
                    }
                ),
                encoding="utf-8",
            )
            (analysis_dir / "correlated-report.json").write_text(
                '{"findings": []}\n', encoding="utf-8"
            )
            receipt = root / "evidence" / "receipt.json"
            receipt.write_text(
                json.dumps(
                    {
                        "schema": "p101-corpus-receipt-v1",
                        "passed": True,
                        "strict": True,
                        "fixtures": str(root),
                        "selected_cases": 1,
                        "completed_cases": 1,
                        "cases": [
                            {
                                "name": "clean",
                                "scenario": "tour",
                                "scenario_behavior": "executable-clean",
                                "status": "PASS",
                                "report": "clean",
                            }
                        ],
                    }
                ),
                encoding="utf-8",
            )
            document, records = BUILD_LAB.load_receipt(receipt, root)
            cases = BUILD_LAB.load_cases(root, receipt, records, None, None)
            rendered = BUILD_LAB.render_markdown(root / "lab", cases, receipt, document["passed"], document["strict"])
            self.assertIn("p101-test corpus verification: PASS (strict)", rendered)
            self.assertIn("Clean lesson", rendered)


if __name__ == "__main__":
    unittest.main()
