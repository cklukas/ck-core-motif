#!/usr/bin/env python3
"""Ensure Xft draw teardown destroys the cached object safely."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
start = text.index("_XmXftDrawDestroy(Display *display, Window window, XftDraw *draw)")
body = text[start:text.index("\n}\n", start)]
cached_pos = body.index("cached_draw = _XmXftDrawCache[i].draw")
destroy_pos = body.index("XftDrawDestroy(cached_draw)")
assert cached_pos < destroy_pos
assert "if (cached_draw != NULL)" in body[cached_pos:destroy_pos]
assert "XftDrawDestroy(draw)" not in body
print("Xft draw teardown destroys the matching cached object")
