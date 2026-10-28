#!/usr/bin/env python3
"""Check I18List first-column restoration cannot narrow-wrap."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "long long new_left_loc;" in source
assert "XmI18List_left_loc(ilist) = bounded_short(" in source
assert "XmI18List_left_loc(ilist) = new_left_loc;" not in source
print("PASS I18List first-column restoration stays bounded")
