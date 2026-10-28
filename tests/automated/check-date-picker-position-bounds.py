#!/usr/bin/env python3
"""Ensure DatePicker popup positioning clamps wide coordinate arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DatePicker.c").read_text(
    encoding="utf-8"
)
start = source.index("static void position_popup")
body = source[start : source.index("\nstatic void calendar_changed", start)]
assert "long long x_pos, y_pos" in body
assert "x_pos < 0" in body and "y_pos < 0" in body
assert "x = (Position)x_pos" in body and "y = (Position)y_pos" in body
print("PASS DatePicker popup positioning clamps wide coordinates")
