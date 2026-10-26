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
assert "XGetGCValues" not in function
assert "XGContextFromGC(" not in function
assert "XmStringExtent(picker->color_picker.render_table, value_string" in function
assert "text_height" in function
assert "((int)picker->core.height - (int)text_height) / 2" in function
print("PASS ColorPicker redisplay has a bounded text baseline")
