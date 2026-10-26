#!/usr/bin/env python3
"""Check I18List column widths and scrollbar totals cannot wrap."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text()
calc_start = source.index("CalcColumnInfo(Widget w, Boolean force)")
calc_end = source.index("\n/*\tFunction Name: ResizeSliders", calc_start)
calc = source[calc_start:calc_end]
resize_start = source.index("ResizeSliders(Widget w)")
resize_end = source.index("\n/*\tFunction Name: VScroll", resize_start)
resize = source[resize_start:resize_end]

assert "max_width = 0;" in calc
assert "if (max_width > SHRT_MAX)" in calc
assert "XmI18List_column_widths(ilist)[i] = (short)max_width;" in calc
assert "int64_t total_width;" in resize
assert "total_width > INT_MAX -" in resize
assert "max_width = (int)total_width;" in resize
print("PASS I18List column geometry uses bounded width arithmetic")
