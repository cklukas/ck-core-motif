#!/usr/bin/env python3
"""Check XmColorPicker Redisplay handles X11 resource failures safely."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorPicker.c").read_text()
start = source.rindex("static void Redisplay(")
end = source.index("static void Events(", start)
function = source[start:end]

assert "!window || !picker->color_picker.gc" in function
assert "if (swatch)" in function
assert "picker->core.height > 10" in function
font_query = function.index("font = XQueryFont")
font_guard = function.index("if (!font)")
font_use = function.index("font->ascent")
assert font_query < font_guard < font_use
print("PASS ColorPicker redisplay failure guards")
