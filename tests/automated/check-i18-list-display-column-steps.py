#!/usr/bin/env python3
"""Check I18List display column steps stay within Position bounds."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "cur_x = bounded_position((long long)cur_x - col_widths[i]);" in source
assert "cur_x = bounded_position((long long)cur_x - HORIZONTAL_SPACE);" in source
assert "cur_x = bounded_position((long long)cur_x + HORIZONTAL_SPACE +" in source
assert "cur_x -= col_widths[i];" not in source
print("PASS I18List display column steps stay bounded")
