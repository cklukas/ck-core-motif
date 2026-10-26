#!/usr/bin/env python3
"""Ensure TextField baseline callbacks validate outputs and allocations."""

from pathlib import Path


source = (Path(__file__).parents[2] / "lib" / "Xm" / "TextF.c").read_text(
    encoding="utf-8"
)
start = source.index("TextFieldGetBaselines(Widget w,")
end = source.index("\nstatic Boolean\nTextFieldGetDisplayRect", start)
region = source[start:end]

assert "w == NULL || baselines == NULL || line_count == NULL" in region
assert "uint64_t baseline;" in region
assert "base_array == NULL" in region
assert "*line_count = 0;" in region
assert "baseline > (uint64_t)((Dimension)~0)" in region
assert "base_array[0] = (Dimension)baseline;" in region

print("TextField baseline callbacks guard outputs and allocations")
