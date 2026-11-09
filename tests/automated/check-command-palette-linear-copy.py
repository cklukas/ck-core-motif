#!/usr/bin/env python3
"""Ensure CommandPalette assembles display labels through XmString APIs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CommandPalette.c").read_text()
start = source.index("static void rebuild")
body = source[start:source.index("static void natural", start)]
display_start = source.index("static XmString make_display_string")
display = source[display_start:start]

assert "make_display_string(" in body
assert "XmStringConcat(separator, shortcut)" in display
assert "XmStringConcat(result, suffix)" in display
assert "strcat(" not in display
assert "sprintf(" not in display
print("PASS CommandPalette label assembly uses XmString composition")
