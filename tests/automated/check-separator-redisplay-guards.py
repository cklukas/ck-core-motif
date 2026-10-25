#!/usr/bin/env python3
"""Check XmSeparator validates its expose-time drawing state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Separator.c").read_text()
start = source.index("static void \nRedisplay(")
end = source.index("static void \nDestroy(", start)
redisplay = source[start:end]

assert "if (!XtIsRealized(wid)" in redisplay
assert "mw->separator.separator_GC == NULL" in redisplay
assert "mw->core.width <= 2 * mw->primitive.highlight_thickness" in redisplay
assert "mw->core.height <= 2 * mw->primitive.highlight_thickness" in redisplay
assert "mw->primitive.top_shadow_GC == NULL" in redisplay
assert "mw->primitive.bottom_shadow_GC == NULL" in redisplay
print("PASS Separator redisplay guards")
