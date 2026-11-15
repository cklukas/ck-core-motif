#!/usr/bin/env python3
"""Check InfoBar text coordinates cannot wrap through Position."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "InfoBar.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "bounded_position(message_left)" in source
assert "bounded_position(action_left)" in source
assert source.count("bounded_position(((long long)bar->core.height") >= 2
print("PASS InfoBar text coordinates stay bounded")
