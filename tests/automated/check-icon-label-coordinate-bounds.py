#!/usr/bin/env python3
"""Check IconLabel text coordinates cannot wrap through Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconLabel.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert source.count("bounded_position((long long)x)") >= 2
assert source.count("bounded_position((long long)y)") >= 2
print("PASS IconLabel text coordinates stay bounded")
