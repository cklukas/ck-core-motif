#!/usr/bin/env python3
"""Check I18List text-rendering baseline arithmetic stays bounded."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "start_y = bounded_position((long long)XmI18List_sep_y(ilist)" in source
assert "start_y += ilist->primitive.shadow_thickness;" not in source
assert "start_y += LINE_HEIGHT;" not in source
print("PASS I18List text baseline stays bounded")
