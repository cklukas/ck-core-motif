#!/usr/bin/env python3
"""Check I18List selected-row fill advancement stays bounded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert source.count("cur_y = bounded_position((long long)cur_y + row_height);") >= 2
assert "\n\tcur_y += row_height;" not in source
print("PASS I18List selected-row fill Y advancement stays bounded")
