#!/usr/bin/env python3
"""Ensure Xft draw creation validates display and drawable inputs."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
start = text.index("_XmXftDrawCreate(Display *display, Window window)")
body = text[start:text.index("\n}\n", start)]
guard_pos = body.index("if (display == NULL || window == None)")
cache_pos = body.index("for (i=0; i<_XmXftDrawCacheSize", guard_pos)
assert guard_pos < cache_pos
print("Xft draw creation validates display and drawable inputs")
