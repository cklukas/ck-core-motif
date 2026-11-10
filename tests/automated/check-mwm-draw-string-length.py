#!/usr/bin/env python3
"""Check that MWM text drawing retains Xlib's signed-length contract."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients/mwm/WmGraphics.c").read_text(
    encoding="utf-8"
)
header = (Path(__file__).parents[2] / "clients/mwm/WmGraphics.h").read_text(
    encoding="utf-8"
)

draw_start = source.index("void DrawStringInBox")
draw_end = source.index("} /* END OF FUNCTION DrawStringInBox */", draw_start)
draw = source[draw_start:draw_end]
assert "stringLengthSize = strlen(str);" in draw
assert "if (stringLengthSize > (size_t)INT_MAX)" in draw
assert "stringLength = (int)stringLengthSize;" in draw
assert draw.index("stringLengthSize = strlen(str);") < draw.index(
    "textWidth = XTextWidth(pfs, str, stringLength);"
)
assert "str, stringLength);" in draw
assert "str, strlen(str));" not in draw

helper_start = source.index("void WmDrawString")
helper_end = source.index("}/* END OF FUNCTION WmDrawString */", helper_start)
helper = source[helper_start:helper_end]
assert "int length" in helper
assert "ACTIVE_PSD == NULL || length < 0" in helper
assert "char *string, int length" in header

print("PASS MWM text drawing bounds lengths before Xlib conversion")
