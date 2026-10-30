#!/usr/bin/env python3
"""Ensure RowActions combines labels as compound XmString segments."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text()
start = source.index("static XmString button_label")
body = source[start:source.index("static void free_items", start)]

assert "XmStringConcat" in body
assert "XmStringCopy(shortcut)" in body
assert "XmStringUnparse" not in body
print("PASS RowActions label assembly preserves compound strings")
