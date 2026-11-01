#!/usr/bin/env python3
"""Check MainWindow menu-bar width queries handle collapsed geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MainW.c").read_text()

assert "int             parent_height, height_change, desired_width;" in source
assert "desired_width = (int) mw->core.width" in source
assert "(2 * (int) mw->mwindow.margin_width)" in source
assert "(desired_width > 0) ? desired_width : 0" in source
start = source.index("static Boolean \nSetValues(")
set_values = source[start:source.index("GetAutoDragVertRects", start)]
assert "new_w->core.width == width && new_w->core.height == height" in set_values
assert "Resize((Widget)new_w);" in set_values
assert set_values.index("Resize((Widget)new_w);") < set_values.index("new_w->core.width = width ;")

print("MainWindow width query and unchanged-geometry relayout are bounded")
