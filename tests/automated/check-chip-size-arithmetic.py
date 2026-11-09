#!/usr/bin/env python3
"""Check Chip natural and layout geometry use explicit bounded conversions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Chip.c").read_text()
assert "(unsigned long)m.horizontal+(unsigned long)m.inset" in source
assert "(unsigned long)m.icon_width+(unsigned long)m.gap" in source
assert "(int)MIN(m->icon_width,(unsigned int)MAX(0,right-left))" in source
assert "(int)MIN(m->icon_height,(unsigned int)available_height)" in source
print("PASS Chip size arithmetic uses bounded conversions")
