#!/usr/bin/env python3
"""Check the first heap motion buffer has room for the boundary entry."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "DragC.c").read_text()
start = source.index("if (mb->count == (STACKMOTIONBUFFERSIZE))")
end = source.index("else  {", start)
body = source[start:end]

assert "(size_t)STACKMOTIONBUFFERSIZE + 1" in body
assert "((size_t)STACKMOTIONBUFFERSIZE + 1) * sizeof(MotionEntryRec)" in body

print("PASS DnD motion buffer transition reserves the boundary entry")
