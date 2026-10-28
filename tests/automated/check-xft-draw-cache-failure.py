#!/usr/bin/env python3
"""Ensure failed Xft draws are not inserted into the cache."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
start = text.index("_XmXftDrawCreate(Display *display, Window window)")
body = text[start:text.index("\n}\n", start)]
null_pos = body.index("if (draw == NULL)")
store_pos = body.index("/* Store it in the cache.", null_pos)
assert null_pos < store_pos
print("Failed Xft draws are not cached")
