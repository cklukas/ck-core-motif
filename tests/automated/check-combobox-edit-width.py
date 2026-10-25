#!/usr/bin/env python3
"""Check ComboBox edit-field sizing avoids unsigned wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()

assert "int available_edit_width, available_edit_height;" in source
assert "available_edit_width = (int) XtWidth(cb) -" in source
assert "available_edit_width -= (int) CB_ArrowSpacing(cb) +" in source
assert "ebW = (Dimension) ((available_edit_width > 0) ? available_edit_width : 1);" in source
assert "ebH = (Dimension) ((available_edit_height > 0) ? available_edit_height : 1);" in source

print("ComboBox edit-field geometry is bounded")
