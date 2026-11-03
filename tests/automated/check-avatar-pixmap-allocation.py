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
face = "face = XCreatePixmap(display, target, diameter, diameter, widget->core.depth);"
assert face in modern and "if (face == None)" in modern
assert modern.index(face) < modern.index("if (face == None)")
assert "_XmAppearanceFillRoundedRectangle(display, target, gc, x, y," in modern

redisplay = body("static void Redisplay(")
text = "Pixmap text = XCreatePixmap(display, window, diameter, diameter,"
assert text in redisplay and "if (text != None)" in redisplay
assert redisplay.index(text) < redisplay.index("if (text != None)")
assert "XFreePixmap(display, text);" in redisplay

print("PASS Avatar temporary pixmap and XRender allocations are guarded")
