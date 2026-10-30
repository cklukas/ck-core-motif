#!/usr/bin/env python3
"""Check FontSelector teardown after partial initialization."""

from pathlib import Path


source = Path("lib/Xm/FontS.c").read_text()
start = source.index("Destroy(Widget w)")
body = source[start:source.index("/*\tFunction Name: SetValues", start)]

assert "if (XmFontS_font_info(fsw) == NULL)" in body
assert "encodings != NULL && *encodings != NULL" in body
assert "i++, encodings++" not in body

print("FontSelector destroy handles partial initialization safely")
