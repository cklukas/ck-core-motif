#!/usr/bin/env python3
"""Ensure Xft creation avoids window-attribute queries for pixmaps."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
start = text.index("_XmXftDrawCreate(Display *display, Window window)")
body = text[start:text.index("\n}\n", start)]
root_pos = body.index("root != None")
attrs_pos = body.index("XGetWindowAttributes(display, window", root_pos)
assert root_pos < attrs_pos
assert "XGetGeometry(display, window" in body
print("Xft creation avoids window attributes on pixmaps")
