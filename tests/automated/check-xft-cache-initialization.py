#!/usr/bin/env python3
"""Ensure newly allocated Xft cache slots are always initialized."""

from pathlib import Path


text = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
start = text.index("_XmXftDrawCreate(Display *display, Window window)")
body = text[start:text.index("\n}\n", start)]
memset_pos = body.index("memset(_XmXftDrawCache + i")
store_pos = body.index("_XmXftDrawCache[i].display = display", memset_pos)
assert memset_pos < store_pos
assert "#ifdef FIX_1449" not in body[memset_pos - 40:store_pos]
print("Xft cache growth initializes every new slot")
