#!/usr/bin/env python3
"""Check SegmentedControl preferred-width arithmetic cannot overflow int."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text()
start = source.index("static void layout(")
body = source[start:source.index("static void Initialize", start)]
assert "unsigned long long pref_total" in body
assert "(unsigned long long)spacing" in body
assert "pref_total <= (unsigned long long)available" in body
print("PASS SegmentedControl layout arithmetic is widened")
