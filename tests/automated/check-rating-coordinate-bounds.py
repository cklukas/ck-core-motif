#!/usr/bin/env python3
"""Check RatingControl star vertices are bounded before XPoint conversion."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "RatingControl.c").read_text(
    encoding="utf-8"
)
start = source.index("static void draw_star")
body = source[start : source.index("static void notify", start)]
assert "x < SHRT_MIN" in body and "x > SHRT_MAX" in body
assert "y < SHRT_MIN" in body and "y > SHRT_MAX" in body
assert "points[i].x = (short)" in body and "points[i].y = (short)" in body
redisplay_start = source.index("static void Redisplay(Widget widget")
redisplay = source[redisplay_start:source.index("static void BufferedRedisplay", redisplay_start)]
assert "(short)MIN(m.inset, SHRT_MAX)" in redisplay
assert "(unsigned short)MIN(width, USHRT_MAX)" in redisplay
for marker in ("static void Highlight(Widget widget)", "static void Unhighlight(Widget widget)"):
    start = source.index(marker)
    body = source[start:source.index("}", start)]
    assert "widget->core.being_destroyed" in body
print("PASS RatingControl star vertices are bounded for XPoint")
