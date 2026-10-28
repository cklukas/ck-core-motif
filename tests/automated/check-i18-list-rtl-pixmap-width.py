#!/usr/bin/env python3
"""Check I18List RTL pixmap placement uses width for the X edge."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "XtWidth(ilist) - XmI18List_left_loc(ilist) - XmI18List_row_data(ilist)[row].pix_width" in source
assert "XtWidth(ilist) - XmI18List_left_loc(ilist) - XmI18List_row_data(ilist)[row].pix_height" not in source
print("PASS I18List RTL pixmap placement uses width")
