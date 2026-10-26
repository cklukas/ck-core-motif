#!/usr/bin/env python3
"""Check ToggleButton default geometry uses widened Dimension arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleB.c").read_text()

assert "static Dimension toggle_bounded_dimension(uint64_t value);" in source
assert "width_total = (uint64_t)new_w->label.TextRect.width" in source
assert "height_total = content_height" in source
assert "new_w->core.width = toggle_bounded_dimension(width_total);" in source
assert "new_w->core.height = toggle_bounded_dimension(height_total);" in source
assert "value > USHRT_MAX" in source
print("PASS ToggleButton default dimensions saturate")
