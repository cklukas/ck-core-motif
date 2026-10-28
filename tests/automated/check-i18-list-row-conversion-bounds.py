#!/usr/bin/env python3
"""Check I18List pixel-to-row conversion clamps calculated rows."""
from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text()
start = source.index("CvtPositionToRowColumn(Widget w")
body = source[start : source.index("/*\tFunction Name: CvtRowColumnToPosition", start)]
assert "*row = bounded_short((long long)IN_COLUMN_HEADER" in body
assert "*row = bounded_short((long long)XmI18List_first_row(ilist)" in body
assert "(short)(pixel_y /" not in body
print("PASS I18List row conversion is bounded")
