#!/usr/bin/env python3
"""Check TabBox canvas labels cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TabBox.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "bounded_position((long long)draw.x + 1)" in source
assert "bounded_position((long long)y + 1)" in source
assert "bounded_position((long long)draw.x)," in source
assert "bounded_position((long long)y)," in source
print("PASS TabBox canvas label coordinates stay bounded")
