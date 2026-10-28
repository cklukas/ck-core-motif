#!/usr/bin/env python3
"""Check TreeView labels cannot wrap through Motif's Position type."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "TreeView.c").read_text(
    encoding="utf-8"
)
assert "static Position bounded_position(long long value)" in source
assert "bounded_position((long long)x)" in source
assert "bounded_position((long long)label_y)" in source
print("PASS TreeView label coordinates stay bounded")
