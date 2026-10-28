#!/usr/bin/env python3
"""Check I18List cell right-edge arithmetic cannot wrap short."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "end = bounded_short((long long)XmI18List_column_widths(ilist)[col] +" in source
assert "end = XmI18List_column_widths(ilist)[col] + start;" not in source
print("PASS I18List cell right-edge arithmetic stays bounded")
