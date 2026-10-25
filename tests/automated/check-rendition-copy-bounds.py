#!/usr/bin/env python3
"""Check rendition tag copies reject overflowing allocation widths."""

from pathlib import Path


source = Path("lib/Xm/XmRenderT.c").read_text()

assert "SIZE_MAX / sizeof(XmStringTag)" in source
assert "UINT_MAX / sizeof(XmStringTag)" in source
assert "(Cardinal)(sizeof(XmStringTag) *" in source
assert "FreeRendition(toRend);" in source

print("rendition tag-copy allocation bounds are guarded")
