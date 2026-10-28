#!/usr/bin/env python3
"""Ensure Xft creation avoids window-attribute queries for pixmaps."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
start = text.index("_XmXftDrawCreate(Display *display, Window window)")
body = text[start:text.index("\n}\n", start)]
fix_start = body.index("#ifdef FIX_1444")
fix_end = body.index("#else", fix_start)
fix_body = body[fix_start:fix_end]
assert "XGetWindowAttributes(display, window" not in fix_body
assert "XGetGeometry(display, window" in fix_body
print("Xft creation avoids window attributes on pixmaps")
