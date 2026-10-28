#!/usr/bin/env python3
"""Ensure List drag snapshots reject stale selected positions."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "List.c").read_text(
    encoding="utf-8"
)
start = source.index("/* CR 8878: selectedItems may contain extraneous items. */")
end = source.index("  else\n    {", start)
body = source[start:end]
assert "valid_count = 0;" in body
assert "selectedPositions[i] > 0" in body
assert "selectedPositions[i] <= lw->list.itemCount" in body
assert "XmStringCopy(lw->list.items[item])" in body
print("PASS List drag snapshots validate selected positions")
