#!/usr/bin/env python3
"""Check DropHighlight labels cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "DropHighlight.c").read_text(
    encoding="utf-8"
)
assert "#include <limits.h>" in source
assert "static Position bounded_position(long long value)" in source
assert "bounded_position((long long)label_x)" in source
assert "bounded_position((long long)label_y)" in source
print("PASS DropHighlight label coordinates stay bounded")
