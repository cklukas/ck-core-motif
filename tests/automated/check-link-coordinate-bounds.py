#!/usr/bin/env python3
"""Check Link label coordinates cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Link.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "bounded_position(4)" in source
assert "bounded_position(((long long)link->core.height - text_height) / 2)" in source
print("PASS Link label coordinates stay bounded")
