#!/usr/bin/env python3
"""Verify XmScale calculates scrollbar spans and slider sizes in wide types."""

from pathlib import Path


source = (Path(__file__).resolve().parents[2] / "lib" / "Xm" / "Scale.c").read_text(
    encoding="utf-8"
)
start = source.index("static void \nCalcScrollBarData(")
end = source.index("static void \nSetScrollBarData(", start)
body = source[start:end]

assert "int64_t scrollbar_size, slider_size_wide;" in body
assert "int64_t size;" in body
assert "slider_size_wide = ((int64_t)SCROLLBAR_MAX / scrollbar_size)" in body
assert "* (int64_t)SLIDER_SIZE(sw);" in body
assert "if (slider_size_wide < 1)" in body
assert "else if (slider_size_wide > SCROLLBAR_MAX)" in body
assert "slider_size = (int)slider_size_wide;" in body
assert "slider_size = (SCROLLBAR_MAX / scrollbar_size) * SLIDER_SIZE( sw);" not in body

value_changed_start = source.index("static void \nValueChanged(")
value_changed_end = source.index("static void \nStartDrag", value_changed_start)
value_changed = source[value_changed_start:value_changed_end]
assert "int scroll_range = SCROLLBAR_MAX - sb->scrollBar.slider_size;" in value_changed
assert "if (scroll_range <= 0)" in value_changed
assert "/ (float)scroll_range;" in value_changed

print("Scale scrollbar arithmetic remains wide until the public int boundary")
