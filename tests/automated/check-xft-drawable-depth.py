#!/usr/bin/env python3
"""Ensure the Xft draw helper selects a drawable-compatible depth path."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
start = text.index("_XmXftDrawCreate(Display *display, Window window)")
body = text[start:text.index("\n}\n", start)]
assert "XGetGeometry(display, window" in body
assert "depth == 1" in body
assert "XftDrawCreateBitmap(display, window)" in body
assert "XGetWindowAttributes(display, window, &wa)" in body
print("Xft draw creation selects a drawable-compatible depth")
