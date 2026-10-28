#!/usr/bin/env python3
"""Check I18List visible-row probing does not wrap widget height."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "bounded_short((long long)ilist->core.height + 1)" in source
assert "(short) (ilist->core.height + 1)" not in source
print("PASS I18List visible-height probe stays bounded")
