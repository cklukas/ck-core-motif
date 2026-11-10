#!/usr/bin/env python3
"""Check that rendition-table growth fits both size_t and Xt Cardinal."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "XmRenderT.c").read_text(encoding="utf-8")
start = source.index("XmRenderTableCvtFromProp")
end = source.index("#endif", start)
region = source[start:end]

assert "SIZE_MAX / sizeof(XmRendition)" in region
assert "UINT_MAX / sizeof(XmRendition)" in region
assert "XtRealloc" in region
cache_start = source.index("typedef struct _XftColorTable")
cache_end = source.index("/* The number of colours cached for a display")
cache_region = source[cache_start:cache_end]
# The Xft colour cache is a fixed-size table per display, so it never grows.
assert "#define XFT_COLOR_CACHE_SIZE 128" in source
assert "XftColorEntry entries[XFT_COLOR_CACHE_SIZE];" in cache_region
assert "(table->next_victim + 1) % XFT_COLOR_CACHE_SIZE" in cache_region
assert "XESetCloseDisplay(display, codes->extension, XftColorTableClose)" in cache_region
assert "XtRealloc" not in cache_region
assert "static XftColor *color_cache" not in source
print("PASS XmRenderTable growth respects Xt allocation width and the Xft colour cache is bounded")
