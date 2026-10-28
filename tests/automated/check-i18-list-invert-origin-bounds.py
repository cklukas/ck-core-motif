#!/usr/bin/env python3
"""Check I18List selection-inversion origins cannot wrap short."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "x = bounded_short(-(long long)width + (long long)XtWidth(w)" in source
assert "x = bounded_short((long long)XmI18List_left_loc(ilist)" in source
assert "x = -width + XtWidth(w) - XmI18List_left_loc(ilist)" not in source
print("PASS I18List inversion origins stay bounded")
