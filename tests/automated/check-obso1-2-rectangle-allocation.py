#!/usr/bin/env python3
"""Check legacy rounded-rectangle buffers fit Xt's allocation size."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib/Xm/Obso1_2.c").read_text(
    encoding="utf-8"
)

assert source.count("UINT_MAX / (sizeof(XRectangle) * 4)") >= 2
assert source.count("new_rects == NULL") >= 2
print("Legacy rounded-rectangle allocation is bounded")
