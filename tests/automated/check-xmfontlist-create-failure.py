#!/usr/bin/env python3
"""Check font-list constructors propagate rendition creation failure."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmFontList.c").read_text()
for name in ("XmFontListCreate(", "XmFontListCreate_r("):
    start = source.index(name)
    region = source[start : start + 1300]
    guard = "if (rends[0] == NULL)"
    assert guard in region
    assert region.index(guard) < region.index("XmRenderTableAddRenditions")
print("PASS font-list constructors stop after failed rendition creation")
