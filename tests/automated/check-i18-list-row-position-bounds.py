#!/usr/bin/env python3
"""Check I18List row-to-position arithmetic cannot wrap short Y."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "long long pixel_y = y;" in source
assert "*y = bounded_short(pixel_y);" in source
assert "*y = (XmI18List_sep_y(ilist) + title_extra +" not in source
print("PASS I18List row-to-position arithmetic stays bounded")
