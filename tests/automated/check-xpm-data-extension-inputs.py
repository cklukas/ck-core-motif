#!/usr/bin/env python3
"""Check XPM data extension sizing validates external metadata pointers."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrDatFrI.c").read_text(
    encoding="utf-8"
)
start = source.index("static int\nCountExtensions(")
body = source[start:source.index("static void\nCreateExtensions", start)]
assert "ext_size == NULL || ext_nlines == NULL" in body
assert "if (num == 0)" in body
assert "if (ext == NULL)" in body
assert "if (ext->name == NULL)" in body
assert "ext->lines == NULL" in body
assert "if (*line == NULL)" in body
print("PASS XPM data extension sizing validates metadata")
