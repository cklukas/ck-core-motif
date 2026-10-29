#!/usr/bin/env python3
"""Check ScrolledWindow scrollbar extents saturate Dimension arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ScrolledW.c").read_text()
start = source.index("GetVariableSize(")
start = source.index("GetVariableSize(\nXmScrolledWindowWidget", start)
end = source.index("GeometryManager(\nWidget", start)
region = source[start:end]

assert "vswidth = scw_bounded_extent((long long)vsb->core.width +" in region
assert "hsheight = scw_bounded_extent((long long)hsb->core.height +" in region
assert "vswidth = vsb->core.width + sw->swindow.pad + vsbht;" not in region
assert "hsheight = hsb->core.height + sw->swindow.pad + hsbht;" not in region
print("PASS ScrolledWindow scrollbar extents use bounded Dimension arithmetic")
