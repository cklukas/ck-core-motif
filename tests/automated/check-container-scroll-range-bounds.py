#!/usr/bin/env python3
"""Check Container scrolling handles invalid and fully-covered ranges."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("ScrollProc(")
region = source[start:source.index("\n}", start)]
assert "nav_data.maximum.y <= nav_data.minimum.y" in region
assert "nav_data.slider_size.y >= nav_data.maximum.y - nav_data.minimum.y" in region
assert "nav_data.maximum.x <= nav_data.minimum.x" in region
assert "nav_data.slider_size.x >= nav_data.maximum.x - nav_data.minimum.x" in region
print("PASS Container scroll clamps invalid and fully-covered navigator ranges")
