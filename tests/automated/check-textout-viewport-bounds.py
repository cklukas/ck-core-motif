#!/usr/bin/env python3
"""Check TextOut offset recalculation clamps margin-only viewports."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextOut.c").read_text()
h_start = source.index("_XmTextChangeHOffset(")
h_end = source.index("_XmTextChangeVOffset(", h_start)
v_end = source.index("_XmTextToggleCursorGC(", h_end)
h_region = source[h_start:h_end]
v_region = source[h_end:v_end]

for region, name in ((h_region, "horizontal"), (v_region, "vertical")):
    assert "(uint64_t)tw->core." in region, f"{name} viewport is not widened"
    assert "?" in region and ": 0;" in region, f"{name} viewport lacks zero clamp"

assert "inner_width = tw->core.width - (2 * margin_width);" not in h_region
assert "inner_height = tw->core.height - (2 * margin_height);" not in v_region
print("PASS TextOut offset recalculation clamps margin-only viewports")
