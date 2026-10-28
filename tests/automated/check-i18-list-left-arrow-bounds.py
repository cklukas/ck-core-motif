#!/usr/bin/env python3
"""Check I18List left-arrow scrolling cannot wrap its horizontal origin."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "XmI18List_left_loc(ilist) = bounded_short(" in source
assert "XmI18List_left_loc(ilist) += 0 - before;" not in source
assert "XmI18List_left_loc(ilist) += XmI18List_column_widths(ilist)[i-1];" not in source
print("PASS I18List left-arrow scrolling stays bounded")
