#!/usr/bin/env python3
"""Check modern SegmentedControl avoids unsigned wrapped draw extents."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "SegmentedControl.c").read_text()
focus = source[source.index("void _XmSegmentedControlDrawFocus("):
               source.index("Boolean _XmSegmentedControlDraw(")]
draw_start = source.index("Boolean _XmSegmentedControlDraw(")
draw = source[draw_start:source.index("static void Initialize", draw_start)]

assert "(unsigned int)child->core.width - 2U" in focus
assert "(unsigned int)child->core.height - 2U" in focus
assert "if (width > 2 * stroke && height > 2 * stroke)" in draw
assert "(unsigned int)(height - 2 * stroke)" in draw
print("PASS SegmentedControl drawing keeps inner extents representable")
