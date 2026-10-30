#!/usr/bin/env python3
"""Check model-widget callbacks fail closed during destruction."""

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
checks = {
    "lib/Xm/DataGrid.c": "grid->core.being_destroyed",
    "lib/Xm/TreeView.c": "view->core.being_destroyed",
    "lib/Xm/Toolbar.c": "toolbar->core.being_destroyed",
}

for relative, marker in checks.items():
    source = (ROOT / relative).read_text(encoding="utf-8")
    assert marker in source, f"missing model callback teardown guard in {relative}"

print("PASS model callbacks guard destruction")
