#!/usr/bin/env python3
"""Ensure CommandPalette validates list positions before indexing its map."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text(
    encoding="utf-8"
)
start = source.index("static void invoke_callback")
body = source[start : source.index("\nWidget XmCreateCommandPalette", start)]
assert "positions[0] < 1" in body
assert "(Cardinal)positions[0] > palette->palette.visible_count" in body
assert "palette->palette.visible[(Cardinal)positions[0] - 1]" in body
lookup = body.index("item = palette->palette.visible[(Cardinal)positions[0] - 1]")
assert "if (item >= palette->palette.item_count) return;" in body[lookup:]
print("PASS CommandPalette validates selected list positions")
