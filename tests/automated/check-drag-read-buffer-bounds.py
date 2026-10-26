#!/usr/bin/env python3
"""Ensure DnD buffer reads avoid overflowing protocol-size arithmetic."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragICC.c").read_text()
start = source.index("_XmReadDragBuffer(")
body = source[start:source.index("/************************************************************************", start + 10)]
assert "size_t\tnumCurr" in body
assert "size_t\tcopy_size" in body
assert "copy_size > buf->size - numCurr" in body
assert "memcpy(ptr, buf->curr, copy_size)" in body
assert "numCurr + size > buf->size" not in body
print("PASS DnD buffer reads use checked size arithmetic")
