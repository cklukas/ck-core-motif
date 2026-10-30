#!/usr/bin/env python3
"""Check public action helpers fail closed during widget destruction."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
checks = {
    "lib/Xm/CommandPalette.c": "palette->core.being_destroyed",
    "lib/Xm/Toast.c": "toast->core.being_destroyed",
}

for relative, marker in checks.items():
    source = (ROOT / relative).read_text(encoding="utf-8")
    assert marker in source, f"missing action teardown guard in {relative}"

print("PASS action helpers guard widget destruction")
