#!/usr/bin/env python3
"""Check both XmDrawnButton shadow paths validate their drawing state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DrawnB.c").read_text()
start = source.index("static void \nRedisplay(")
end = source.index("SetValuesPrehook(", start)
section = source[start:end]

assert "XtIsRealized((Widget) db)" in section
assert "db->primitive.top_shadow_GC != NULL" in section
assert "db->primitive.bottom_shadow_GC != NULL" in section
assert "db->primitive.shadow_thickness != 0" in section
assert "db->core.width > 2 * db->primitive.highlight_thickness" in section
assert "db->core.height > 2 * db->primitive.highlight_thickness" in section
assert "else if (CanDrawShadow(db))" in section
assert "if (!CanDrawShadow(db))" in section
print("PASS DrawnButton shadow redisplay guards")
