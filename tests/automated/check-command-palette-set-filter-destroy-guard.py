#!/usr/bin/env python3
"""Ensure filtering does not reach a child during CommandPalette teardown."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text()
start = source.index("void XmCommandPaletteSetFilter")
end = source.index("char *XmCommandPaletteGetFilter", start)
body = source[start:end]

guard = body.index("widget->core.being_destroyed")
forward = body.index("XmSearchBoxSetString")
assert guard < forward
assert "palette->palette.search != NULL" in body
print("PASS CommandPalette filtering rejects destroying widgets")
