#!/usr/bin/env python3
"""Check that TabBox geometry and image-cache growth is failure-safe."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabBox.c").read_text()

layout_start = source.index("\nLayout(XmTabBoxWidget tab)")
layout = source[layout_start:source.index("\n    switch( XmTabBox_tab_mode(tab) )", layout_start)]
assert "if( count <= 0 )" in layout
assert "XiTabRect *actual;" in layout
assert "actual = (XiTabRect*)" in layout
assert "UINT_MAX / sizeof(*actual)" in layout
assert "if (actual == NULL)" in layout
assert layout.index("if (actual == NULL)") < layout.index("XmTabBox__actual(tab) = actual")
assert layout.index("XmTabBox__actual(tab) = actual") < layout.index("XmTabBox__num_actual(tab) = count")

geometry_start = source.index("CalcTabGeometry(XmTabBoxWidget tab)")
geometry_end = source.index("\n/*", geometry_start)
geometry = source[geometry_start:geometry_end]
assert "if( count <= 0 )" in geometry
assert "XRectangle *wanted;" in geometry
assert "UINT_MAX / sizeof(*wanted)" in geometry
assert "if (wanted == NULL)" in geometry
assert geometry.index("if (wanted == NULL)") < geometry.index("XmTabBox__wanted(tab) = wanted")

cache_start = source.index("ResetImageCache(XmTabBoxWidget tab)")
cache_end = source.index("\nstatic void", cache_start + 20)
cache = source[cache_start:cache_end]
assert "XiCache *cache;" in cache
assert "UINT_MAX / sizeof(*cache)" in cache
assert "XtMalloc((Cardinal)(sizeof(*cache) *" in cache
assert "if (cache == NULL)" in cache
assert "XmTabBox__cache(tab) = NULL;" in cache
assert "XtFree((XtPointer)XmTabBox__cache(tab));" in cache

print("PASS TabBox geometry and cache allocation failures are handled")
