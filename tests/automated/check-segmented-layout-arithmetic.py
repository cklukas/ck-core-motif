#!/usr/bin/env python3
"""Check SegmentedControl preferred-width arithmetic cannot overflow int."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text()
start = source.index("static void allocate_segments(")
body = source[start:source.index("static void Initialize", start)]
assert "uint64_t natural = 0, gaps, usable, extra" in body
assert "(uint64_t)spacing * (count - 1)" in body
assert "natural + gaps > available" in body
print("PASS SegmentedControl layout arithmetic is widened")
