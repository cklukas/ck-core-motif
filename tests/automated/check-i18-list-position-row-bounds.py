#!/usr/bin/env python3
"""Check I18List position-to-row conversion does not mutate short Y."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "long long pixel_y = y;" in source
assert "pixel_y -= XmI18List_sep_y(ilist)" in source
assert "\n        y -= XmI18List_sep_y(ilist)" not in source
print("PASS I18List position-to-row arithmetic stays wide")
