#!/usr/bin/env python3
"""Check DropDown child placement is bounded during collapsed resize."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropDown.c").read_text()

assert "DropDownChildSpan(int available, Dimension border)" in source
assert "child_height = (int)rheight" in source
assert "if (child_height < 0)" in source
assert "(int)rwidth - (int)label_width" in source
assert "DropDownChildSpan(child_height, text_geom.border_width)" in source
assert "XmDropDown_text_x(cbw) = dropdown_bounded_position(text_x);" in source
assert "dropdown_bounded_position(label_x)" in source
assert "dropdown_bounded_position(text_y)" in source
assert "dropdown_bounded_position(arrow_x)" in source
assert "*text_width = dropdown_bounded_dimension(" in source

print("DropDown child geometry is bounded")
