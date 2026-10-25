#!/usr/bin/env python3
"""Check XmArrowButton avoids unsafe drawing with missing resources."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ArrowB.c").read_text()
redisplay = source[source.index("static void \nRedisplay("):source.index("static void \nDestroy(")]
draw_arrow = source[source.index("static void\nDrawArrow("):]

assert "aw->primitive.top_shadow_GC != NULL" in redisplay
assert "aw->primitive.bottom_shadow_GC != NULL" in redisplay
assert "if (width == 0 || height == 0 || top_gc == NULL || bottom_gc == NULL)" in draw_arrow
assert "if (!center_gc && aw->arrowbutton.detail_shadow_thickness == 1)" in draw_arrow
print("PASS ArrowButton redisplay guards")
