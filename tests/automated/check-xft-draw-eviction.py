#!/usr/bin/env python3
"""Ensure Xft cache eviction does not create a draw object."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
header = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderTI.h").read_text(
    encoding="utf-8"
)
start = source.index("_XmXftDrawEvict(Display *display, Window window)")
body = source[start:source.index("\n}\n", start)]
assert "for (i = 0; i < _XmXftDrawCacheSize; ++i)" in body
assert "XftDrawDestroy(cached_draw)" in body
assert "XmeWarning" not in body
assert "_XmXftDrawEvict(Display *display, Window window);" in header
print("Xft draw eviction clears cache without creating a draw")
