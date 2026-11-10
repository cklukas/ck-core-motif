#!/usr/bin/env python3
"""Ensure ProgressBar uses logical-leading geometry for its fill and clips."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "ProgressBar.c").read_text(
    encoding="utf-8"
)

assert "int bar_x, bar_y, bar_height, fill_x;" in source
assert "XmDirectionMatchPartial(XmPrim_layout_direction(progress)," in source
assert "? bar_x + bar_width - (int)fill_width : bar_x;" in source
assert "fill_gc,fill_x,bar_y,fill_width" in source
assert "XFillRectangle(display,window,progress->progress.fill_gc,fill_x" in source
assert "XRectangle part={bounded_position((long long)fill_x)" in source
assert "fill_x==bar_x?bar_x+fill_width:bar_x" in source
print("ProgressBar fill and percentage clips mirror in RTL")
