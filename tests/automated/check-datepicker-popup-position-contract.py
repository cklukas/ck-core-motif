#!/usr/bin/env python3
"""Ensure the DatePicker popup test tracks the wide-position implementation."""
from pathlib import Path

source = (Path(__file__).parents[2] / "tests" / "automated" /
          "check-datepicker-popup-position.py").read_text()
assert "x_pos = 0;" in source
assert "y_pos = 0;" in source
assert "x_pos > (long long)WidthOfScreen(screen) - popup_width" in source
assert "y_pos > (long long)HeightOfScreen(screen) - popup_height" in source
print("PASS DatePicker popup regression tracks wide positions")
