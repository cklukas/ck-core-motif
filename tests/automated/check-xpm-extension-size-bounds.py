#!/usr/bin/env python3
"""Check XPM extension sizing rejects null inputs and arithmetic overflow."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XpmCrBufFrI.c").read_text(
    encoding="utf-8"
)
start = source.index("static unsigned int\nExtensionsSize")
body = source[start:source.index("static void\nWriteExtensions", start)]
assert "if (ext == NULL)" in body
assert "ext->name == NULL" in body
assert "ext->lines == NULL" in body
assert "line_length > UINT_MAX - size - 11" in body
assert "line_length > UINT_MAX - size - 4" in body
assert "size > UINT_MAX - 11" in body
assert "size > UINT_MAX - 4" in body
caller = source[source.index("/* compute the extensions"):source.index("/* write the header line", source.index("/* compute the extensions"))]
assert "if (extensions && ext_size == 0)" in caller
print("PASS XPM extension sizing is null- and overflow-safe")
