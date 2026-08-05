"""Read the C playground scenario manifest without duplicating its inventory."""

from __future__ import annotations

import re
from dataclasses import dataclass
from pathlib import Path


SCENARIO_PATTERN = re.compile(
    r'^P101_SCENARIO\(\s*([A-Z0-9_]+)\s*,\s*"([^"]+)"\s*,\s*'
    r'(P101_SCENARIO_[A-Z_]+)\s*,\s*"([^"]+)"\s*\)$'
)


@dataclass(frozen=True)
class ScenarioDefinition:
    identifier: str
    name: str
    behavior: str
    description: str


def load_scenario_manifest(root: Path) -> dict[str, ScenarioDefinition]:
    path = root / "include" / "playground_scenarios.def"
    definitions: dict[str, ScenarioDefinition] = {}

    for line_number, raw_line in enumerate(path.read_text(encoding="utf-8").splitlines(), 1):
        line = raw_line.strip()
        if not line or line.startswith("/*") or line.startswith("*") or line.startswith("*/"):
            continue
        match = SCENARIO_PATTERN.fullmatch(line)
        if match is None:
            raise ValueError(f"{path}:{line_number}: invalid scenario manifest row")
        identifier, name, behavior_token, description = match.groups()
        behavior = behavior_token.removeprefix("P101_SCENARIO_").lower().replace("_", "-")
        if name in definitions:
            raise ValueError(f"{path}:{line_number}: duplicate scenario name {name}")
        definitions[name] = ScenarioDefinition(identifier, name, behavior, description)

    if not definitions:
        raise ValueError(f"{path}: scenario manifest is empty")
    return definitions
