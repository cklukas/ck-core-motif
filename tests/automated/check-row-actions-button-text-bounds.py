#!/usr/bin/env python3
"""Check RowActions builds labels as compound strings without text truncation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RowActions.c").read_text(encoding="utf-8")
start = source.index("static XmString button_label")
end = source.index("static void free_items", start)
region = source[start:end]

assert "XmStringConcat" in region
assert "XmStringCopy(shortcut)" in region
assert "XmStringUnparse" not in region
print("PASS RowActions button labels preserve compound strings")
