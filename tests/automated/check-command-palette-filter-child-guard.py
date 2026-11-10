#!/usr/bin/env python3
"""Ensure the CommandPalette filter getter tolerates child construction failure."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text(
    encoding="utf-8"
)
start = source.index("char *XmCommandPaletteGetFilter")
body = source[start : source.index("void XmCommandPaletteShow", start)]
assert "palette->palette.search ? XmSearchBoxGetString(palette->palette.search) : NULL" in body
print("PASS CommandPalette filter getter guards an absent SearchBox child")
