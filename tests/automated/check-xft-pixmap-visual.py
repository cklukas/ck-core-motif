#!/usr/bin/env python3
"""Ensure Xft pixmaps use a visual matching their depth."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
start = text.index("_XmXftDrawCreate(Display *display, Window window)")
body = text[start:text.index("\n}\n", start)]
assert "Bool geometry_ok" in body
assert "XGetVisualInfo(display," in body
assert "VisualScreenMask | VisualDepthMask" in body
assert "vtemplate.depth = (int)depth" in body
assert "vtemplate.screen = drawable_screen" in body
assert "visual = DefaultVisual(display, drawable_screen)" in body
assert "colormap = DefaultColormap(display, drawable_screen)" in body
assert "XFree((char *)visuals)" in body
print("Xft pixmap visuals match drawable depth")
