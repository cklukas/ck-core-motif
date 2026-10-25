#!/usr/bin/env python3
"""Check PushButtonGadget unhighlight clearing fits the widget."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "PushBG.c").read_text(encoding="utf-8")
start = source.index("static void \nBorderUnhighlight(")
end = source.index("static void \nKeySelect(", start)
region = source[start:end]
assert "pb->rectangle.width > 2 * pb->gadget.highlight_thickness" in region
assert "pb->rectangle.height > 2 * pb->gadget.highlight_thickness" in region
assert region.count("pb->rectangle.width <= 2 * delta") >= 1
assert region.count("pb->rectangle.height <= 2 * delta") >= 1
assert "XmeClearBorder" in region
print("push-button gadget unhighlight geometry is bounded")
