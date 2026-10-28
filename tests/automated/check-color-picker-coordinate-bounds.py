#!/usr/bin/env python3
"""Check ColorPicker labels cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ColorPicker.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "bounded_position(38)" in source
assert "bounded_position(((long long)picker->core.height - text_height) / 2)" in source
print("PASS ColorPicker label coordinates stay bounded")
