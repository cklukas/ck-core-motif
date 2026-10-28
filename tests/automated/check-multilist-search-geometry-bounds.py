#!/usr/bin/env python3
"""Check MultiList search-row geometry uses bounded arithmetic."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "MultiList.c").read_text()

assert "multilist_position(long long value)" in source
assert "multilist_dimension(unsigned long value)" in source
layout = source[source.index("static void PositionFindAndFindText") : source.index("/************************************************************\n * \n * Functions that Create Widgets.")]
assert "multilist_dimension((unsigned long)find->core.height" in layout
assert "multilist_dimension((unsigned long)find->core.width" in layout
assert "multilist_position((long long)height" in layout
assert "long long text_origin" in layout
assert "text_width = width - temp_val" not in layout
assert "find_y = (Position) height" not in layout
print("PASS MultiList search-row geometry is bounded")
