#!/usr/bin/env python3
"""Check MainWindow menu-bar width queries handle collapsed geometry."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MainW.c").read_text()

assert "int             parent_height, height_change, desired_width;" in source
assert "desired_width = (int) mw->core.width" in source
assert "(2 * (int) mw->mwindow.margin_width)" in source
assert "(desired_width > 0) ? desired_width : 0" in source

print("MainWindow width query arithmetic is bounded")
