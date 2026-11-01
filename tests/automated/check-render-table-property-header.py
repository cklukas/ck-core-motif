#!/usr/bin/env python3
"""Check rendition-property headers use the checked output buffer."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
start = source.index("XmRenderTableCvtToProp(")
body = source[start:source.index("/* Now iterate over the list of renditions */", start)]
assert "CVTtransfervector" not in source
assert "CVTtvinited" not in source
assert "buffer[0] = '\\0';" in body
assert "CVTaddString(buffer, CVTproperties[i], size);" in body
assert 'CVTaddString(buffer, ",", 1);' in body
assert 'CVTaddString(buffer, "\\n", 1);' in body
print("PASS rendition property header uses checked dynamic output")
