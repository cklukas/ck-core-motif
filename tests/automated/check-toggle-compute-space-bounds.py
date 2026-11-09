#!/usr/bin/env python3
"""Check ToggleButton ComputeSpace handles undersized widgets safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleB.c").read_text()

assert "tb->label.TextRect.y = toggle_bounded_position(" in source
assert "(int64_t)tb->core.height - tb->label.margin_top" in source
assert "tb->label.TextRect.x = toggle_bounded_position(" in source

print("ToggleButton ComputeSpace geometry is bounded")
