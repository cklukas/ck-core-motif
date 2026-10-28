#!/usr/bin/env python3
"""Check I18List text centering uses signed wide offset arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "long long text_offset =" in source
assert "cur_y = bounded_position((long long)cur_y + text_offset);" in source
assert "cur_y -= (int)(XmI18List_row_height(ilist)" not in source
print("PASS I18List text centering stays signed")
