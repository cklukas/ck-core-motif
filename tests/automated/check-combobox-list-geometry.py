#!/usr/bin/env python3
"""Check ComboBox scrolled-list geometry avoids unsigned wraparound."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ComboBox.c").read_text()

assert "int available_list_width, available_list_height;" in source
assert "available_list_height = (int) XtHeight(cb) -" in source
assert "available_list_width = (int) XtWidth(cb) -" in source
assert "listH = (htLeft > ebH) ? htLeft - ebH : 1;" in source

print("ComboBox scrolled-list geometry is bounded")
