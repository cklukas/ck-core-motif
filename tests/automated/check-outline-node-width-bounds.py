#!/usr/bin/env python3
"""Check Outline child-width negotiation cannot wrap past the right edge."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Outline.c").read_text(encoding="utf-8")
start = source.index("\nNegotiateNodeWidth(Widget w, OutlineConstraints node)\n")
start = source.index("{", start)
end = source.index("\n}\n", start)
region = source[start:end]
assert "int width_avail;" in region
assert "(int)w->core.width -" in region
assert "(int)XmOutlineC_widget_x(node)" in region
assert "if (width_avail < 1)" in region
assert "width_avail = 1;" in region
assert "Dimension width_avail;" not in region
print("outline child-width negotiation is signed and clamped")
