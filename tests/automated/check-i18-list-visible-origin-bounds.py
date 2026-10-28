#!/usr/bin/env python3
"""Check I18List visible-origin adjustment cannot wrap short."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "before = bounded_short((long long)before -" in source
assert "before -= XmI18List_left_loc(ilist);" not in source
print("PASS I18List visible-origin adjustment stays bounded")
