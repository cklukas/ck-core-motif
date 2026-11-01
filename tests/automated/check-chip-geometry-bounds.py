#!/usr/bin/env python3
"""Check bounded XmChip geometry calculations."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Chip.c").read_text(encoding="utf-8")
start = source.index("static void metrics")
end = source.index("static XRectangle rectangle", start)
region = source[start:end]

assert "chip->chip.label && chip->chip.font" in region
assert "MIN(USHRT_MAX,MAX((unsigned long)*height,w))" in region
assert "MIN(USHRT_MAX,MAX(1UL,h))" in region
print("PASS chip geometry is bounded")
