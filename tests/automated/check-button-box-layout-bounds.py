#!/usr/bin/env python3
"""Check ButtonBox keeps layout coordinates and Xt request modes bounded."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ButtonBox.c").read_text(
    encoding="utf-8"
)

for fragment in (
    "static Position bounded_position(long long value)",
    "if (value < SHRT_MIN)",
    "if (value > SHRT_MAX)",
    "(request->request_mode & XtCWQueryOnly) != 0",
    "(XtGeometryMask)(*mask & (Mask)(CWWidth | CWHeight))",
    "pos_minor = bounded_position(((long long)box_minor - child_minor) / 2 +",
    "pos_x = bounded_position((long long)box_major -",
    "pos_y = bounded_position((long long)pos_major);",
):
    assert fragment in source, fragment

print("ButtonBox layout coordinates and geometry request modes are bounded")
