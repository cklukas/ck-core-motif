#!/usr/bin/env python3
"""Check DnD location cleanup tolerates malformed completion callbacks."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
for name, marker, callback_arg in (
    ("Container.c", "FreeLocationData(Widget wid", "cs"),
    ("TextSel.c", "FreeLocationData(Widget w", "ts"),
    ("TextFSel.c", "FreeLocationData(Widget w", "ts"),
):
    source = (root / name).read_text()
    start = source.index(marker)
    end = source.index("\n}\n", start) + 3
    body = source[start:end]
    assert f"{callback_arg} == NULL" in body, name
    assert "if (ds == NULL)" in body, name
    assert "location_data != NULL" in body, name

print("transfer location cleanup validates completion callback state")
