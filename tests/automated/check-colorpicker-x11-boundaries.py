#!/usr/bin/env python3
"""Check ColorPicker keeps checked geometry through X11 boundaries."""

from pathlib import Path


root = Path(__file__).parents[2] / "lib" / "Xm"
picker = (root / "ColorPicker.c").read_text(encoding="utf-8")
space = (root / "ColorSpaceI.h").read_text(encoding="utf-8")

assert "hsv[1]=hi != 0.0 ? d/hi : 0.0" in space
assert "static Dimension bounded_x_extent(int value)" in picker
assert "static Boolean status_is_true(Status status)" in picker
assert "XtConfigureWidget(child,x,y,bounded_x_extent(width)," in picker
assert "bounded_x_extent(MIN(tile,width-x))" in picker
assert "bounded_x_extent(MIN(tile,height-y))" in picker
assert "visual->map_entries > 0 ? MIN(visual->map_entries,256) : 0" in picker
assert "for(int j=0;j<count;++j)available[j].pixel=(unsigned long)j;" in picker
assert "if(width<=0||height<=0||attr.depth<=0)return;" in picker
assert "(unsigned int)attr.depth" in picker
assert "bounded_x_extent(2*r)" in picker
assert "bounded_x_extent(width-2*inset)" in picker

print("PASS ColorPicker bounds X11 extents, depth, and palette fallback inputs")
