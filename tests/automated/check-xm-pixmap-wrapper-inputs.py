#!/usr/bin/env python3
"""Check public pixmap wrappers reject absent widget/screen/name inputs."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ImageCache.c").read_text()

def body(name: str, next_name: str) -> str:
    start = source.index(name + "(")
    start = source.index("\n{", start) + 2
    end = source.index(next_name + "(", start)
    return source[start:end]

scaled = body("XmGetScaledPixmap", "XmGetPixmapByDepth")
assert "widget == NULL || image_name == NULL" in scaled
assert scaled.index("widget == NULL || image_name == NULL") < scaled.index("XtWidgetToApplicationContext")

by_depth = body("XmGetPixmapByDepth", "XmGetPixmap")
assert "screen == NULL || image_name == NULL" in by_depth
assert by_depth.index("screen == NULL || image_name == NULL") < by_depth.index("XtDisplayToApplicationContext")

pixmap = body("XmGetPixmap", "XmeGetMask")
assert "screen == NULL || image_name == NULL" in pixmap
assert pixmap.index("screen == NULL || image_name == NULL") < pixmap.index("XtDisplayToApplicationContext")

print("PASS Xm pixmap wrapper input guards")
