#!/usr/bin/env python3
"""Check IconLabel text coordinates cannot wrap through Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "IconLabel.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "bounded_position(left)" in source
assert "bounded_position(((long long)label->core.height - (long long)value_height) / 2)" in source
print("PASS IconLabel text coordinates stay bounded")
