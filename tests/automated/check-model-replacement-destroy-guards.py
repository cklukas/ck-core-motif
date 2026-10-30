#!/usr/bin/env python3
"""Check model replacement rejects widgets entering Xt teardown."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
checks = {
    "lib/Xm/DataGrid.c": "widget->core.being_destroyed",
    "lib/Xm/TreeView.c": "widget->core.being_destroyed",
    "lib/Xm/CommandPalette.c": "widget->core.being_destroyed",
    "lib/Xm/Toolbar.c": "widget->core.being_destroyed",
}

for relative, marker in checks.items():
    source = (ROOT / relative).read_text(encoding="utf-8")
    assert marker in source, f"missing model replacement guard in {relative}"

print("PASS model replacement guards destruction")
