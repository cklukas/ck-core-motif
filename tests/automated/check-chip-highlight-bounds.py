#!/usr/bin/env python3
"""Check XmChip highlight geometry is guarded against unsigned underflow."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Chip.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Redisplay(Widget widget")
body = source[start : source.index("static Boolean SetValues", start)]
guard = ("chip->primitive.highlighted && chip->primitive.highlight_thickness &&"
         "\n        chip->core.width > 2 * chip->primitive.highlight_thickness &&"
         "\n        chip->core.height > 2 * chip->primitive.highlight_thickness")
assert guard in body
print("PASS Chip highlight rectangle is bounded for tiny widgets")
