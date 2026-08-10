#!/usr/bin/env python3
"""Regression tests for playground lab tool discovery."""

from __future__ import annotations

import importlib.util
import os
import sys
import tempfile
import unittest
from pathlib import Path
from unittest.mock import patch


MODULE_PATH = Path(__file__).resolve().parents[1] / "corpus" / "build-lab.py"
sys.path.insert(0, str(MODULE_PATH.parent))
SPEC = importlib.util.spec_from_file_location("p101_build_lab", MODULE_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(f"cannot load {MODULE_PATH}")
BUILD_LAB = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = BUILD_LAB
SPEC.loader.exec_module(BUILD_LAB)


class AuditDoctorDiscoveryTests(unittest.TestCase):
    def test_qualified_host_tool_environment_is_admitted(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            tool = Path(directory) / "audit-doctor"
            tool.write_text("#!/bin/sh\nexit 0\n", encoding="utf-8")
            tool.chmod(0o755)
            with patch.dict(os.environ, {"P101_AUDIT_DOCTOR": str(tool)}):
                self.assertEqual(BUILD_LAB.find_audit_doctor(Path(directory)), tool.resolve())

    def test_unusable_environment_override_is_not_admitted(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            missing = Path(directory) / "missing-audit-doctor"
            with patch.dict(os.environ, {"P101_AUDIT_DOCTOR": str(missing)}):
                with patch.object(BUILD_LAB.shutil, "which", return_value=None):
                    self.assertIsNone(BUILD_LAB.find_audit_doctor(Path(directory)))


if __name__ == "__main__":
    unittest.main()
