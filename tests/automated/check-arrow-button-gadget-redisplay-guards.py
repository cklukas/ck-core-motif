#!/usr/bin/env python3
"""Check XmArrowButtonGadget validates all drawing prerequisites."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ArrowBG.c").read_text()
start = source.index("static void \nRedisplay(")
end = source.index("static void\nDrawArrowG(", start)
redisplay = source[start:end]
helper = source[end:]

assert "aw->arrowbutton.background_GC != NULL" in redisplay
assert "background_width > 0 && background_height > 0" in redisplay
assert "aw->arrowbutton.top_shadow_GC != NULL" in redisplay
assert "aw->arrowbutton.bottom_shadow_GC != NULL" in redisplay
assert "width == 0 || height == 0 || top_gc == NULL || bottom_gc == NULL" in helper
assert "!center_gc && ag->arrowbutton.detail_shadow_thickness == 1" in helper
print("PASS ArrowButtonGadget redisplay guards")
