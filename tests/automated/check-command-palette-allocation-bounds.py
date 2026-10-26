#!/usr/bin/env python3
"""Check CommandPalette item-array allocation bounds."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text()
start = source.index("void XmCommandPaletteSetItems")
body = source[start:source.index("Cardinal XmCommandPaletteGetItemCount", start)]
assert "SIZE_MAX / sizeof(*palette->palette.items)" in body
assert "UINT_MAX / sizeof(*palette->palette.items)" in body
assert "item array is too large" in body
assert "XtCalloc(\n        count, sizeof(*palette->palette.items))" in body
print("PASS CommandPalette item-array allocation bounds")
