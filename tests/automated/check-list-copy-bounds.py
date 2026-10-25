#!/usr/bin/env python3
"""Check list copy-on-write helpers guard count products and allocations."""
from pathlib import Path

source = Path(__file__).parents[2] / "lib" / "Xm" / "List.c"
text = source.read_text(encoding="utf-8")
for name in ("CopyItems", "CopySelectedItems", "CopySelectedPositions"):
    start = text.index("\n" + name + "(XmListWidget", text.index("* " + name + " -"))
    end = text.index("\n}\n", start) + 2
    region = text[start:end]
    assert "SIZE_MAX / sizeof" in region, name
    assert "== NULL" in region, name
print("list copy helpers guard count products and allocations")
