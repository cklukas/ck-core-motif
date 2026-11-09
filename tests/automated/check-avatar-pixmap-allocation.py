#!/usr/bin/env python3
"""Check Avatar stops cleanly after temporary X resource allocation failures."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Avatar.c").read_text(
    encoding="utf-8"
)

def body(signature: str) -> str:
    start = source.index(signature)
    return source[start:source.index("\nstatic ", start + len(signature))]

composite = body("static Boolean composite_circle(")
diameter_guard = "if (diameter <= 0 || diameter > 2048 ||"
stride_guard = "if (image->bytes_per_line <= 0 ||\n        (size_t)diameter > UINT_MAX / (size_t)image->bytes_per_line)"
assert diameter_guard in composite
assert stride_guard in composite
assert "image->data = XtCalloc((Cardinal)diameter,\n                           (Cardinal)image->bytes_per_line);" in composite
assert composite.index(diameter_guard) < composite.index("image = XCreateImage")
assert composite.index(stride_guard) < composite.index("image->data = XtCalloc")
assert source.count("unsigned int extent;") >= 2
assert source.count("extent = (unsigned int)diameter;") >= 2
assert "return XGetGeometry(XtDisplay((Widget)avatar), avatar->avatar.pixmap," in source
assert "&root, &x, &y, width, height, &border, depth) != 0;" in source
for allocation, guard in (
    ("mask = XCreatePixmap(display, target, diameter, diameter, 8);",
     "if (mask == None)"),
    ("gc = XCreateGC(display, mask, 0, NULL);", "if (gc == NULL)"),
    ("source = XRenderCreatePicture(display, face, format, 0, NULL);",
     "if (source == None)"),
    ("destination = XRenderCreatePicture(display, target, format, 0, NULL);",
     "if (destination == None)"),
    ("coverage = XRenderCreatePicture(display, mask, alpha, 0, NULL);",
     "if (coverage == None)"),
):
    assert allocation in composite and guard in composite
    assert composite.index(allocation) < composite.index(guard)

modern = body("static void PaintModernAvatar(")
face = "face = XCreatePixmap(display, target, extent, extent, widget->core.depth);"
assert face in modern and "if (face == None)" in modern
assert modern.index(face) < modern.index("if (face == None)")
assert "_XmAppearanceFillRoundedRectangle(display, target, gc, x, y," in modern

redisplay = body("static void Redisplay(")
text = "Pixmap text = XCreatePixmap(display, window, extent, extent,"
assert text in redisplay and "if (text != None)" in redisplay
assert redisplay.index(text) < redisplay.index("if (text != None)")
assert "XFreePixmap(display, text);" in redisplay

print("PASS Avatar temporary pixmap and XRender allocations are guarded")
