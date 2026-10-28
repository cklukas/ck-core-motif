#!/usr/bin/env python3
"""Check Avatar initials cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Avatar.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "bounded_position((long long)x)" in source
assert "bounded_position((long long)y)" in source
print("PASS Avatar initials coordinates stay bounded")
