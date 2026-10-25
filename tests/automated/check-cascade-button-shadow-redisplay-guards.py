#!/usr/bin/env python3
"""Check XmCascadeButton validates its armed shadow draw state."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "CascadeB.c").read_text()
start = source.index("static void \nDrawShadow(")
end = source.index("static void \nDrawCascade(", start)
shadow = source[start:end]

assert "XtIsRealized((Widget)cb)" in shadow
assert "cb->primitive.top_shadow_GC != NULL" in shadow
assert "cb->primitive.bottom_shadow_GC != NULL" in shadow
assert "cb->primitive.shadow_thickness != 0" in shadow
assert "cb->core.width > 2 * cb->primitive.highlight_thickness" in shadow
assert "cb->core.height > 2 * cb->primitive.highlight_thickness" in shadow
print("PASS CascadeButton shadow redisplay guards")
