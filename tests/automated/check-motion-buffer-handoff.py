#!/usr/bin/env python3
"""Ensure motion-buffer growth is propagated back to the caller."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text()
start = source.index("static void \nUpdateMotionBuffer(")
body = source[start:source.index("static void \nDragMotionProto", start)]
assert "MotionBuffer *mbp" in body
assert "MotionBuffer mb = *mbp;" in body
assert "*mbp = mb;" in body
assert "UpdateMotionBuffer(dc, &motionBuffer" in source
print("PASS motion-buffer growth is handed back to the caller")
