#!/usr/bin/env python3
"""Check I18List display fill origins use bounded coordinates."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert source.count("bounded_position(-(long long)tot_width + XtWidth(w) -") >= 2
assert source.count("bounded_position((long long)XmI18List_left_loc(ilist) -") >= 2
assert "? -tot_width + XtWidth(w) - XmI18List_left_loc(ilist)" not in source
print("PASS I18List display fill origins stay bounded")
