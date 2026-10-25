#!/usr/bin/env python3
"""Check that traversal focus helpers tolerate missing focus data."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "Traversal.c").read_text(
    encoding="utf-8"
)
for name in ("_XmDestroyFocusData(", "_XmSetActiveTabGroup("):
    start = text.index(name)
    body = text[start : text.index("}\n", start) + 2]
    if "focusData == NULL" not in body:
        raise SystemExit(f"focus-data guard is missing: {name}")

print("traversal focus-data guards present")
