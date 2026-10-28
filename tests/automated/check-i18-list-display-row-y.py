#!/usr/bin/env python3
"""Check I18List display row baselines and advancement stay bounded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "cur_y = bounded_position((long long)XmI18List_sep_y(ilist)" in source
assert "cur_y = bounded_position((long long)cur_y + row_height);" in source
assert "cur_y = (XmI18List_sep_y(ilist) + VERTICAL_SPACE/2 +" not in source
assert source.count("cur_y += row_height;") == 1
print("PASS I18List display row Y arithmetic stays bounded")
