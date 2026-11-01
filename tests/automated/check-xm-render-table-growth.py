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
cache_start = source.index("static XftColor\nGetCachedXftColor")
cache_end = source.index("#endif", cache_start)
cache_region = source[cache_start:cache_end]
assert "SIZE_MAX / sizeof(XftColor)" in cache_region
assert "UINT_MAX / sizeof(XftColor)" in cache_region
assert "XftColor *grown_cache;" in cache_region
assert "grown_cache = (XftColor *) XtRealloc" in cache_region
assert "color_cache = grown_cache;" in cache_region
assert "if (grown_cache != NULL)" in cache_region
assert "color_cache = (XftColor *) XtRealloc" not in cache_region
assert cache_region.index("if (grown_cache != NULL)") < cache_region.index(
    "color_cache = grown_cache;"
)
print("PASS XmRenderTable growth respects Xt allocation width")
