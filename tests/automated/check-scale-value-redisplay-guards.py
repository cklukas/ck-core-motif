#!/usr/bin/env python3
"""Check XmScale validates value-label state before redisplay."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Scale.c").read_text()
start = source.index("static void \nShowValue(")
end = source.index("static void \nCalcScrollBarData(", start)
show_value = source[start:end]

assert "!XtIsRealized((Widget)sw)" in show_value
assert "sw->scale.value_region == NULL" in show_value
assert "sw->scale.foreground_GC == NULL" in show_value
assert "sw->composite.num_children < 2" in show_value
assert "sw->composite.children[1] == NULL" in show_value
assert "sw->scale.font_list == NULL" in show_value or "sw->scale.font_struct == NULL" in show_value
assert "width = ScaleBoundedDimension(width);" in show_value
assert "height = ScaleBoundedDimension(height);" in show_value
assert "x = ScaleBoundedPosition(x);" in show_value
assert "y = ScaleBoundedPosition(y);" in show_value
assert "ScaleBoundedPosition((int64_t)x + width_return.lbearing)" in show_value
assert "ScaleBoundedPosition((int64_t)y - height + 1)" in show_value
assert "old_x = ScaleBoundedPosition(x);" in show_value
assert "old_width = ScaleBoundedDimension(width);" in show_value
assert "XClearArea(XtDisplay(sw), XtWindow(sw), old_x, old_y, old_width," in show_value
print("PASS Scale value redisplay guards")
