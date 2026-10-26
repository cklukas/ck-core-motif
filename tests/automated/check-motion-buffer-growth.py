#!/usr/bin/env python3
"""Ensure motion-buffer growth checks arithmetic and allocation results."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text()
start = source.index("static void \nUpdateMotionBuffer(")
body = source[start:source.index("static void \nDragMotionProto", start)]
assert "(size_t)-1 - sizeof(MotionBufferRec)" in body
assert "size > (size_t)UINT_MAX" in body
assert body.count("newMb == NULL") >= 2
assert "mb->count > UINT_MAX - STACKMOTIONBUFFERSIZE" in body
print("PASS motion-buffer growth checks arithmetic and allocation")
