#!/usr/bin/env python3
"""Check I18List header/icon baselines use bounded positions."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "cur_y = bounded_position((long long)XmI18List_sep_y(ilist)" in source
assert "cur_y += LINE_HEIGHT;" not in source
assert "cur_y += ilist->primitive.shadow_thickness;" not in source
print("PASS I18List header and icon baselines stay bounded")
