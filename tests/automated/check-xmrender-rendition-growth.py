#!/usr/bin/env python3
from pathlib import Path

source = Path("lib/Xm/XmRenderT.c").read_text()
start = source.index("XmRenderTableCvtFromProp")
end = source.index("void\nXmRenderTableGetDefaultFontExtents", start)
body = source[start:end]
assert "if (rarray == NULL)" in body
assert "int new_max;" in body
assert "SIZE_MAX / sizeof(XmRendition)" in body
assert "if (new_array == NULL)" in body
assert "rarray_max = new_max;" in body
extents_start = end
extents_end = source.index("\n}", extents_start)
extents = source[extents_start:extents_end]
assert "if (rendertable == NULL)" in extents
assert "if (height) *height = 0;" in extents
assert "if (ascent) *ascent = 0;" in extents
assert "if (descent) *descent = 0;" in extents
print("XmRenderTable rendition-array growth guards present")
