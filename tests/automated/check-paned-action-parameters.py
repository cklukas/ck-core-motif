#!/usr/bin/env python3
"""Check that legacy Paned sash increments reject overflowing text safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Paned.c").read_text()

assert "static short\nParseSashIncrement" in source
assert "value < SHRT_MIN || value > SHRT_MAX" in source
assert "ParseSashIncrement(call_data->params[1], &increment_valid)" in source
assert "atoi(call_data->params[1])" not in source
print("Paned sash action parameters use checked increment parsing")
