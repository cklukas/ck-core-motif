#!/usr/bin/env python3
"""Check ComboBox hit and drawing dimensions remain bounded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()

assert "int available_height;" in source
assert "ebH = (Dimension) ((available_height > 0) ? available_height : 1);" in source
assert "if (max_height < 1)" in source
assert "Dimension shadow_width = (XtWidth(widget) > 2 * offset) ?" in source
assert "Dimension shadow_height = (XtHeight(widget) > 2 * offset) ?" in source

print("ComboBox hit and drawing dimensions are bounded")
