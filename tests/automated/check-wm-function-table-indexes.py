#!/usr/bin/env python3
"""Check MWM function-table loops use size-compatible indexes."""
from pathlib import Path


source = (Path(__file__).parents[2] / "clients" / "mwm" / "WmResParse.c").read_text(
    encoding="utf-8"
)
menu_start = source.index("static MenuItem *ParseMenuItems")
menu_end = source.index("} /* END OF FUNCTION ParseMenuItems", menu_start)
menu = source[menu_start:menu_end]
assert "size_t          ix = 0;" in menu

mask_start = source.index("Boolean SetGreyedContextAndMgtMask")
mask_end = source.index("\n\n\n\f", mask_start)
mask = source[mask_start:mask_end]
assert "size_t ix;" in mask
assert "int ix;" not in mask
print("PASS MWM function-table indexes match table-size arithmetic")
