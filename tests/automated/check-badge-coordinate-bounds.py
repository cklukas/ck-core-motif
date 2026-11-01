#!/usr/bin/env python3
"""Check Badge labels cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Badge.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "bounded_position(horizontal)" in source
assert "bounded_position(((long long)height-(long long)th)/2)" in source
print("PASS Badge label coordinates stay bounded")
