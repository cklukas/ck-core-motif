#!/usr/bin/env python3
"""Ensure clipboard property chunks keep a stable type and format."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "CutPaste.c").read_text()
start = source.index("static int \nGetWindowProperty(")
body = source[start:source.index("static void \nClipboardReplaceItem", start)]
assert "Atom expected_type" in body
assert "int expected_format" in body
assert "if (!have_format)" in body
assert "loc_type != expected_type || loc_format != expected_format" in body
print("PASS clipboard chunks keep a consistent type and format")
