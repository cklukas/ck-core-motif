#!/usr/bin/env python3
"""Check far-side ScrollBar arrow positions avoid unsigned wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrollBar.c").read_text()

assert "int arrow_pos;" in source
assert "(int) sbw->core.width - ht - st -" in source
assert "(int) sbw->core.height - ht - st -" in source
assert "sbw->scrollBar.arrow1_x = (Position) arrow_pos;" in source
assert "sbw->scrollBar.arrow1_y = (Position) arrow_pos;" in source

print("ScrollBar far-side arrow positions are signed")
