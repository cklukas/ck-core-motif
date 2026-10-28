#!/usr/bin/env python3
"""Check I18List slider height arithmetic cannot underflow Dimension."""

from pathlib import Path

source = (Path(__file__).parents[2] / "lib" / "Xm" / "I18List.c").read_text(
    encoding="utf-8"
)
assert "long long height;" in source
assert "height = (long long)ilist->core.height -" in source
assert "height = ilist->core.height -" not in source
print("PASS I18List slider height stays signed")
