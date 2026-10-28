#!/usr/bin/env python3
"""Check I18List text coordinates cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert source.count("bounded_position((long long)cur_x)") >= 2
assert source.count("bounded_position((long long)cur_y)") >= 2
print("PASS I18List text coordinates stay bounded")
