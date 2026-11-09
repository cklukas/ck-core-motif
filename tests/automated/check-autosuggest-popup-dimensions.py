#!/usr/bin/env python3
"""Ensure bounded popup dimensions cross Xt's Dimension boundary explicitly."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "AutoSuggestBox.c").read_text()
assert "screen_width = MIN(WidthOfScreen(XtScreen(shell)), SHRT_MAX);" in source
assert "screen_height = MIN(HeightOfScreen(XtScreen(shell)), SHRT_MAX);" in source
assert "(Dimension)width, (Dimension)height" in source
print("PASS AutoSuggestBox popup dimensions are bounded before Xt configuration")
