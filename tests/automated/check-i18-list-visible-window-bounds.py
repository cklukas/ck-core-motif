#!/usr/bin/env python3
"""Check I18List visible-window movement uses wide boundary arithmetic."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "long long visible_end = (long long)before + (long long)ilist->core.width;" in source
assert "if ((long long)start <= visible_end)" in source
assert "move = last - (before + ilist->core.width);" not in source
assert "(width - (before + ilist->core.width))" not in source
print("PASS I18List visible-window arithmetic stays wide")
