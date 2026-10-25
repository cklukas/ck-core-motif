#!/usr/bin/env python3
"""Check ToggleButton ComputeSpace handles undersized widgets safely."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleB.c").read_text()

assert "((int)tb->core.height - (int)tb->label.margin_top" in source
assert "tb->label.TextRect.x = (short)((int)tb->core.width" in source

print("ToggleButton ComputeSpace geometry is bounded")
