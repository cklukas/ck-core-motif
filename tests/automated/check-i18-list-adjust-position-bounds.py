#!/usr/bin/env python3
"""Check I18List visibility movement updates cannot wrap short."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "position = bounded_short(-(long long)value);" in source
assert "position = bounded_short(-(long long)position);" in source
assert "XmI18List_left_loc(ilist) = bounded_short(" in source
assert "XmI18List_left_loc(ilist) += position;" not in source
print("PASS I18List visibility movement stays bounded")
