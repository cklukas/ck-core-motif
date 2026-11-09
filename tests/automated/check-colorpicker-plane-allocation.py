#!/usr/bin/env python3
"""Check ColorPicker's cached swatch plane rejects invalid X drawables."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorPicker.c").read_text(
    encoding="utf-8"
)
initialize_start = source.index("static void Initialize(Widget request,")
initialize_end = source.index("\nstatic ", initialize_start + 1)
initialize = source[initialize_start:initialize_end]
start = source.index("static void paint_plane(Widget child,")
end = source.index("\nstatic ", start + 1)
plane = source[start:end]
pencil_start = source.index("static void paint_pencil(Widget child,")
pencil_end = source.find("\nstatic ", pencil_start + 1)
if pencil_end < 0:
    pencil_end = len(source)
pencil = source[pencil_start:pencil_end]

zero_guard = "if(width==0||height==0)return;"
pixmap = "p->color_picker.plane_cache=XCreatePixmap(d,window,width,height,attr.depth);"
failure_guard = "if(p->color_picker.plane_cache==None)return;"

assert zero_guard in plane, "zero-sized swatch planes must not issue X allocations"
assert pixmap in plane and failure_guard in plane, (
    "a failed swatch pixmap allocation must stop before XPutImage/XCopyArea"
)
assert plane.index(zero_guard) < plane.index(pixmap) < plane.index(failure_guard)
layout_guard = "if(image->bytes_per_line<=0 ||\n           (size_t)strip>SIZE_MAX/(size_t)image->bytes_per_line)"
assert layout_guard in plane, "ColorPicker must reject invalid or overflowing XImage strides"
assert "image->data=calloc((size_t)strip,(size_t)image->bytes_per_line);" in plane
assert plane.index(layout_guard) < plane.index("image->data=calloc"), (
    "ColorPicker must validate an XImage stride before allocating its data"
)
gc = "p->color_picker.swatch_gc=XCreateGC(XtDisplay(widget),drawable,0,NULL);"
assert "p->color_picker.swatch_gc=NULL;" in initialize
assert "if(drawable!=None){" in initialize and gc in initialize
assert initialize.index("if(drawable!=None){") < initialize.index(gc)
assert "if(!p->color_picker.swatch_gc)return;" in plane
assert "GC gc=p->color_picker.swatch_gc;" in pencil and "if(!gc)return;" in pencil
assert pencil.index("GC gc=p->color_picker.swatch_gc;") < pencil.index("if(!gc)return;")
print("PASS ColorPicker plane and swatch drawing allocations are guarded")
