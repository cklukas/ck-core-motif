#!/usr/bin/env python3
"""Check destination callbacks validate their entry arguments."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
for name, marker in (
    ("Container.c", "ContainerDestinationProc(\n        Widget"),
    ("TextSel.c", "static void \nTextDestinationCallback(Widget"),
    ("TextFSel.c", "static void \nTextFieldDestinationCallback(Widget"),
):
    source = (root / name).read_text()
    start = source.index(marker)
    body = source[start:source.index("\n}\n", start)]
    assert "transfer_id == NULL" in body, name
    assert "return;" in body, name

print("DnD destination callbacks validate entry arguments")
