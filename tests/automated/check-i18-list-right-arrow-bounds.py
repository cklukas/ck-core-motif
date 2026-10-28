#!/usr/bin/env python3
"""Check I18List right-arrow scrolling cannot wrap its horizontal origin."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "-((long long)width - (long long)ilist->core.width)" in source
assert "XmI18List_left_loc(ilist) -= end;" not in source
assert "0 - (width - ilist->core.width)" not in source
print("PASS I18List right-arrow scrolling stays bounded")
