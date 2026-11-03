#!/usr/bin/env python3
"""Check ArrowButton drawing bounds its inset geometry before X11 use."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
button = (root / "ArrowB.c").read_text(encoding="utf-8")
gadget = (root / "ArrowBG.c").read_text(encoding="utf-8")

for fragment in (
    "static Position arrow_bounded_position(long long value)",
    "static Dimension arrow_bounded_dimension(unsigned long value)",
    "x = arrow_bounded_position(margin);",
    "width = arrow_bounded_dimension((unsigned long)aw->core.width -",
    "y = arrow_bounded_position(margin);",
    "height = arrow_bounded_dimension((unsigned long)aw->core.height -",
):
    assert fragment in button, fragment

for fragment in (
    "static Position arrowg_bounded_position(long long value)",
    "static Dimension arrowg_bounded_dimension(unsigned long value)",
    "x = arrowg_bounded_position((long long)ag->rectangle.x + margin);",
    "width = arrowg_bounded_dimension((unsigned long)ag->rectangle.width -",
    "y = arrowg_bounded_position((long long)ag->rectangle.y + margin);",
    "height = arrowg_bounded_dimension((unsigned long)ag->rectangle.height -",
):
    assert fragment in gadget, fragment

print("ArrowButton widget and gadget inset drawing stays within X11 bounds")
