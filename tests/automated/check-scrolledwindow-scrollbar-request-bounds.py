#!/usr/bin/env python3
"""Check ScrolledWindow scrollbar resize requests avoid unsigned wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text()

assert "int             parentWidth, parentHeight;" in source
assert "parentWidth += (int) request->width - (int) w->core.width;" in source
assert "parentHeight += (int) request->height - (int) w->core.height;" in source
assert "(parentWidth > 0) ? parentWidth : 0" in source
assert "(parentHeight > 0) ? parentHeight : 0" in source

print("ScrolledWindow scrollbar resize requests are bounded")
