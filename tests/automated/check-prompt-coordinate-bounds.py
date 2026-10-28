#!/usr/bin/env python3
"""Check Prompt text coordinates cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "Prompt.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "bounded_position(4)" in source
assert "bounded_position(((long long)prompt->core.height - height) / 2)" in source
print("PASS Prompt text coordinates stay bounded")
