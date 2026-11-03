#!/usr/bin/env python3
"""Check ColorPicker's cached swatch plane rejects invalid X drawables."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorPicker.c").read_text(
    encoding="utf-8"
)
start = source.index("static void paint_plane(Widget child,")
end = source.index("\nstatic ", start + 1)
plane = source[start:end]

zero_guard = "if(width==0||height==0)return;"
pixmap = "p->color_picker.plane_cache=XCreatePixmap(d,window,width,height,attr.depth);"
failure_guard = "if(p->color_picker.plane_cache==None)return;"

assert zero_guard in plane, "zero-sized swatch planes must not issue X allocations"
assert pixmap in plane and failure_guard in plane, (
    "a failed swatch pixmap allocation must stop before XPutImage/XCopyArea"
)
assert plane.index(zero_guard) < plane.index(pixmap) < plane.index(failure_guard)
print("PASS ColorPicker plane allocation guards invalid X drawables")
