#!/usr/bin/env python3
"""Check NumberBox highlight geometry is safe for tiny widgets."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "NumberBox.c").read_text(
    encoding="utf-8"
)
start = source.index("static void Redisplay(Widget widget")
body = source[start : source.index("static void commit_text", start)]
guard = ("number->primitive.highlighted && number->primitive.highlight_thickness &&"
         "\n        number->core.width > 2 * number->primitive.highlight_thickness &&"
         "\n        number->core.height > 2 * number->primitive.highlight_thickness")
assert guard in body
print("PASS NumberBox highlight rectangle is bounded for tiny widgets")
