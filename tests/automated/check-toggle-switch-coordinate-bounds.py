#!/usr/bin/env python3
"""Check ToggleSwitch labels cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "ToggleSwitch.c").read_text(
    encoding="utf-8"
)
assert "#include <limits.h>" in source
assert "static Position bounded_position(long long value)" in source
assert "bounded_position((long long)label_x)" in source
assert "bounded_position((long long)label_y)" in source
print("PASS ToggleSwitch label coordinates stay bounded")
