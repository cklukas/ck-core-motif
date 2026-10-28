#!/usr/bin/env python3
"""Check I18List display column origins are bounded in both directions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "cur_x = bounded_position((long long)width -" in source
assert "cur_x = bounded_position(XmI18List_left_loc(ilist));" in source
assert "cur_x = width - XmI18List_left_loc(ilist);" not in source
print("PASS I18List display column origins stay bounded")
