#!/usr/bin/env python3
"""Check PushButtonGadget highlight arithmetic and tiny-widget guards."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushBG.c").read_text(encoding="utf-8")
start = source.index("static void\nDrawBorderHighlight(")
end = source.index("static void \nBorderUnhighlight(", start)
region = source[start:end]
assert "int highlight_width" in region
assert "pb->gadget.highlight_thickness <= Xm3D_ENHANCE_PIXEL" in region
assert "pb->rectangle.width <= 2 * delta" in region
assert "pb->rectangle.height <= 2 * delta" in region
assert region.index("pb->rectangle.width <= 2 * delta") < region.index("width = pb->rectangle.width - 2 * delta")
print("push-button gadget highlight geometry is bounded")
