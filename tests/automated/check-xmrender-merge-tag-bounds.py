#!/usr/bin/env python3
"""Ensure rendition merging handles tag counts without signed underflow."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(
    encoding="utf-8"
)
start = source.index("_XmRenditionMerge(")
body = source[start:source.index("\nXmRendition", start + 1)]
assert "unsigned int\ti;" in body
assert "if (tag_count != 0 && tags == NULL)" in body
assert "for (i = tag_count; i != 0; --i)" in body
assert "tags[i - 1]" in body
print("PASS XmRenderTable rendition merge tag bounds")
