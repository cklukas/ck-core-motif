#!/usr/bin/env python3
"""Ensure DnD buffer write offsets cannot wrap the CARD16 ABI result."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("_XmWriteDragBuffer(")
body = source[start:source.index("/************************************************************************", start + 10)]
assert "buf->size > UINT16_MAX" in body
assert "returnVal = buf->size > UINT16_MAX ? UINT16_MAX" in body
print("PASS DnD buffer write offsets stay within CARD16")
