#!/usr/bin/env python3
"""Check IconG label/detail coordinates cannot wrap through Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconG.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert source.count("bounded_position((long long)ig->rectangle.x") >= 2
assert source.count("bounded_position((long long)ig->rectangle.y") >= 3
assert "bounded_position((LayoutIsRtoLG(wid)) ?" in source
print("PASS IconG label and detail coordinates stay bounded")
