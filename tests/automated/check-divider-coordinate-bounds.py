#!/usr/bin/env python3
"""Check Divider labels cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Divider.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "bounded_position((long long)x)" in source
assert "bounded_position(((long long)divider->core.height - label_height) / 2)" in source
print("PASS Divider label coordinates stay bounded")
