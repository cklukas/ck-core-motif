#!/usr/bin/env python3
"""Ensure DnD buffer reads do not wrap their CARD16 byte count."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("_XmReadDragBuffer(")
body = source[start:source.index("/************************************************************************", start + 10)]
assert "if (copy_size > UINT16_MAX)" in body
assert "copy_size = UINT16_MAX;" in body
assert "return (CARD16)copy_size;" in body
print("PASS DnD buffer read counts stay within CARD16")
