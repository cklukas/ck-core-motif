#!/usr/bin/env python3
"""Check XmPushButton focus highlight arithmetic and resources."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushB.c").read_text()
start = source.index("static void\nDrawBorderHighlight(")
end = source.index("static void \nBorderUnhighlight(", start)
highlight = source[start:end]

assert "int highlight_width" in highlight
assert "pb->primitive.highlight_thickness <= Xm3D_ENHANCE_PIXEL" in highlight
assert "pb->primitive.highlight_GC == NULL" in highlight
assert "pb->core.width <= 2 * delta" in highlight
assert "pb->core.height <= 2 * delta" in highlight
assert highlight.index("pb->core.width <= 2 * delta") < highlight.index("width = pb->core.width - 2 * delta")
print("PASS PushButton highlight bounds")
