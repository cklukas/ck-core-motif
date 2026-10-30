#!/usr/bin/env python3
"""Check Container scroll increments use checked wide arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Container.c").read_text(encoding="utf-8")
start = source.rindex("ScrollProc(")
region = source[start:source.index("\n}", start)]
assert region.count("AdjustIntSaturated") >= 4
assert "nav_data.value.y += nav_data.increment.y" not in region
assert "nav_data.value.x += nav_data.increment.x" not in region
print("PASS Container scroll increments use saturating arithmetic")
