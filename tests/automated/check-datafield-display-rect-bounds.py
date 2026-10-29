#!/usr/bin/env python3
"""Check DataField display rectangles use widened bounded geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DataF.c").read_text()
start = source.index("DataFieldGetDisplayRect(\n        Widget w,\n        XRectangle * display_rect )")
start = source.index("{", start)
end = source.index("\n\n/* ARGSUSED */", start)
region = source[start:end]

assert "if (display_rect == NULL)" in region
assert "uint64_t margin_width" in region
assert "uint64_t margin_top" in region
assert "uint64_t margin_bottom" in region
assert "margin_width <= (uint64_t)tf->core.width / 2" in region
assert "margin_top + margin_bottom < (uint64_t)tf->core.height" in region
assert "Position margin_width" not in region
print("PASS DataField display rectangles use widened bounded geometry")
