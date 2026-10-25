#!/usr/bin/env python3
"""Check XmFrame does not draw incomplete cached shadow state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Frame.c").read_text()
start = source.index("static void\nDrawShadow(")
end = source.index("static void\nClearShadow(", start)
draw_shadow = source[start:end]

assert "XtIsRealized((Widget)fw)" in draw_shadow
assert "fw->manager.top_shadow_GC != NULL" in draw_shadow
assert "fw->manager.bottom_shadow_GC != NULL" in draw_shadow
assert "fw->frame.old_width != 0" in draw_shadow
assert "fw->frame.old_height != 0" in draw_shadow
assert "fw->frame.old_shadow_thickness != 0" in draw_shadow
print("PASS Frame shadow redisplay guards")
